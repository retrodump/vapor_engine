/************************************************************************
*
* vapor3D Editor � (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#include "PCH.h"
#include "ScenePage.h"
#include "Events.h"
#include "Editor.h"
#include "EditorIcons.h"
#include "Viewframe.h"
#include "UndoManager.h"
#include "UndoOperation.h"

namespace vapor { namespace editor {

//-----------------------------------//

class NodeItemData : public wxTreeItemData
{
public:

	NodeWeakPtr node;
	ComponentWeakPtr component;
};

//-----------------------------------//

enum 
{
	ID_MenuSceneNodeDelete = wxID_HIGHEST+472,
	ID_MenuSceneNodeDuplicate,
	ID_MenuSceneNodeVisible,
	ID_MenuSceneNodeWireframe,
};

//-----------------------------------//

struct ComponentEntry
{
	bool show;
	const Type& type;
	const unsigned char* icon;
	const int icon_length;
};

#define TYPE(t) t::getType()
#define BMP(s) s, sizeof(s)

static ComponentEntry components[] = {
	{ true, TYPE(Transform),			BMP(chart_line) },
	{ true, TYPE(Model),				BMP(shape_flip_horizontal) },
	{ true, TYPE(Light),				BMP(lightbulb_off) },
	{ true, TYPE(Terrain),				BMP(world) },
	{ true, TYPE(Skydome),				BMP(skydome) },
	{ true, TYPE(Camera),				BMP(camera) },
	//{ true, TYPE(Sound),				BMP(sound) },
	//{ true, TYPE(Listener),			BMP(status_online) },
	{ true, TYPE(Grid),					BMP(grid_icon_white_bg) },
	{ true, TYPE(Geometry),				BMP(shape_flip_horizontal) },
	{ true, TYPE(CharacterController),	BMP(link) },
	{ true, TYPE(FirstPersonController),BMP(camera) },
	{ true, TYPE(ThirdPersonController),BMP(camera) },
	{ true, TYPE(ScriptController),			BMP(shape_flip_horizontal) },
	{ true, TYPE(BoxShape),				BMP(link) },
	{ true, TYPE(MeshShape),			BMP(link) },
	{ true, TYPE(CapsuleShape),			BMP(link) },
	{ true, TYPE(Body),					BMP(link) },
	{ false,TYPE(Scene),				BMP(sitemap_color) }
};

//-----------------------------------//

ScenePage::ScenePage( EditorFrame* frame, wxWindow* parent, wxWindowID id )
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(250, -1) )
	, treeCtrl(nullptr)
	, imageList(nullptr)
	, buttonNodeAdd(nullptr)
	, buttonNodeDelete(nullptr)
	, currentMenu(nullptr)
	, sentLastSelectionEvent(false)
	, editor(frame)
	, engine(editor->getEngine())
	, nodeCounter(0)
{
	initTree();
	initButtons();
	initIcons();
}

//-----------------------------------//

ScenePage::~ScenePage()
{
	cleanScene();
}

//-----------------------------------//

void ScenePage::cleanScene()
{
	ScenePtr scene = weakScene.lock();

	if( !scene )
		return;
	
	scene->onNodeAdded -= fd::bind( &ScenePage::onNodeAdded, this );
	scene->onNodeRemoved -= fd::bind( &ScenePage::onNodeRemoved, this );

	weakScene.reset();
}

//-----------------------------------//

void ScenePage::initTree()
{
	sizer = new wxBoxSizer( wxVERTICAL );

	treeCtrl = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition,
		wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_EDIT_LABELS 
		| wxTR_NO_BUTTONS | wxTR_SINGLE | wxTR_HIDE_ROOT );

	sizer->Add( treeCtrl, 1, wxEXPAND, 0 );

	treeCtrl->Bind(wxEVT_COMMAND_TREE_ITEM_MENU, &ScenePage::onItemMenu, this);
	treeCtrl->Bind(wxEVT_COMMAND_TREE_SEL_CHANGED, &ScenePage::onItemChanged, this);
	treeCtrl->Bind(wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, &ScenePage::onLabelEditBegin, this);
	treeCtrl->Bind(wxEVT_COMMAND_TREE_END_LABEL_EDIT, &ScenePage::onLabelEditEnd, this);
	treeCtrl->Bind(wxEVT_COMMAND_TREE_BEGIN_DRAG, &ScenePage::onDragBegin, this);
	treeCtrl->Bind(wxEVT_COMMAND_TREE_END_DRAG, &ScenePage::onDragEnd, this);

	SetSizer( sizer );

	Bind(wxEVT_CONTEXT_MENU, &ScenePage::onContextMenu, this);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &ScenePage::onMenuSelected, this);
}

//-----------------------------------//

void ScenePage::initButtons()
{
	wxBoxSizer* sizer2 = new wxBoxSizer( wxHORIZONTAL );

	// Search control.
	wxStaticBitmap* iconFind = new wxStaticBitmap( this, wxID_ANY, wxMEMORY_BITMAP(find) );
	sizer2->Add( iconFind, 0, wxALL|wxEXPAND, 5 );
	
	wxTextCtrl* textFind = new wxTextCtrl( this, wxID_ANY );
	sizer2->Add( textFind, 1, wxEXPAND|wxALL, 5 );
	
	// Node delete button.
	buttonNodeDelete = new wxBitmapButton( this, wxID_ANY, wxMEMORY_BITMAP(package_delete) );
	buttonNodeDelete->SetBitmapDisabled( wxMEMORY_BITMAP(package_delete_disable) );
	sizer2->Add( buttonNodeDelete, 0, wxEXPAND|wxTOP|wxBOTTOM, 5 );
	
	buttonNodeDelete->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ScenePage::onButtonNodeDelete, this);
	buttonNodeDelete->Bind(wxEVT_UPDATE_UI, &ScenePage::onButtonNodeDeleteUpdate, this);

	// Node add button.
	buttonNodeAdd = new wxBitmapButton( this, wxID_ANY, wxMEMORY_BITMAP(package_add) );
	sizer2->Add( buttonNodeAdd, 0, wxTOP|wxBOTTOM, 5 );

	buttonNodeAdd->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ScenePage::onButtonNodeAdd, this);
	sizer->Add( sizer2, 0, wxEXPAND, 0 );
}

//-----------------------------------//

void ScenePage::initIcons()
{
	// Create a new list with all the icons.
	imageList = new wxImageList(16, 16, false, 16);
	imageList->Add(wxMEMORY_BITMAP(package));

	foreach( const ComponentEntry& c, components )
	{
		const Type* type = &c.type;
		
		bitmaps[type] = _wxConvertMemoryToBitmap(c.icon, c.icon_length);
		icons[type] = imageList->Add(bitmaps[type]);
	}

	treeCtrl->AssignImageList(imageList);
}

//-----------------------------------//

void ScenePage::setScene(const ScenePtr& scene)
{
	cleanScene();

	weakScene = scene;

	scene->onNodeAdded += fd::bind( &ScenePage::onNodeAdded, this );
	scene->onNodeRemoved += fd::bind( &ScenePage::onNodeRemoved, this );

	// Add the root node.
	treeCtrl->DeleteAllItems();

	wxString sceneName( scene->getName() );
	rootId = treeCtrl->AddRoot(sceneName.Capitalize(), 1);

	addGroup( rootId, scene );
}

//-----------------------------------//

void ScenePage::addGroup( wxTreeItemId id, const NodePtr& node )
{
	if( !node->getInstanceType().inherits<Group>() )
	{
		addNode(id, node);
		return;
	}
	
	GroupPtr group = std::dynamic_pointer_cast<Group>(node);

	foreach( const NodePtr& child, group->getChildren() )
		addGroup(id, child);
}

//-----------------------------------//

wxTreeItemId ScenePage::addNode( wxTreeItemId id, const NodePtr& node )
{
	wxTreeItemId nodeId = treeCtrl->AppendItem( id, node->getName(), 0 );

	foreach( const ComponentMapPair& component, node->getComponents() )
		addComponent(nodeId, component.second);

	NodeItemData* data = new NodeItemData();
	data->node = node;

	treeCtrl->SetItemData( nodeId, data );
	nodeIds[node] = nodeId;

	return nodeId;
}

//-----------------------------------//

void ScenePage::addComponent( wxTreeItemId id, ComponentPtr component )
{
	assert( component != nullptr );
	const Type& type = component->getInstanceType();

	wxTreeItemId componentId = 
		treeCtrl->AppendItem( id, type.getName(), icons[&type] );

	NodeItemData* data = new NodeItemData();
	data->component = component;

	treeCtrl->SetItemData( componentId, data );
}

//-----------------------------------//

NodePtr ScenePage::getNodeFromTreeId( wxTreeItemId id )
{
	if( !id || !id.IsOk() )
		return NodePtr();

	NodeItemData* data = (NodeItemData*) treeCtrl->GetItemData(id);
	assert( data != nullptr );

	return data->node.lock();
}

//-----------------------------------//

wxTreeItemId ScenePage::getTreeIdFromNode(const NodePtr& node)  
{
	NodeIdsMap::iterator it = nodeIds.find(node);

	if( it == nodeIds.end() )
		return wxTreeItemId();

	return nodeIds[node];
}

//-----------------------------------//

void ScenePage::onItemChanged(wxTreeEvent& event)
{
	wxTreeItemId oldId = event.GetOldItem();
	wxTreeItemId newId = event.GetItem();

	const NodePtr& oldNode = getNodeFromTreeId( oldId );
	const NodePtr& newNode = getNodeFromTreeId( newId );
	
	if( newNode )
		buttonNodeDelete->Enable();
	else
		buttonNodeDelete->Disable();

	sentLastSelectionEvent = true;

	// Send events to plugins.
	Events* events = editor->getEventManager();

	if( oldNode )
		events->onNodeUnselect(oldNode);

	if( newNode )
		events->onNodeSelect(newNode);

	sentLastSelectionEvent = false;
}

//-----------------------------------//

class NodeOperation : public UndoOperation
{
public:

	void redo();
	void undo();

	ScenePage* scenePage;
	NodeWeakPtr weakNode;
};

void NodeOperation::redo()
{
	NodePtr node = weakNode.lock();

	if( !node )
		return;
}

void NodeOperation::undo()
{
	NodePtr node = weakNode.lock();

	if( !node )
		return;
}

//-----------------------------------//

void ScenePage::onButtonNodeAdd(wxCommandEvent&)
{
	std::string name("SceneNode"+String::fromNumber(nodeCounter++));
	NodePtr node( new Node(name) );
	node->addTransform();
	
	// This is to prevent the editor of processing the node
	// in the method 'onNodeAdded', which could result in the
	// duplication of the node in the editor's scene tree.
	node->setTag( EditorTags::EditorOnly, true );
	
	ScenePtr scene = weakScene.lock();
	assert( scene != nullptr );
	scene->add(node);

	node->setTag( EditorTags::EditorOnly, false );

	wxTreeItemId id = addNode(rootId, node);

	treeCtrl->Expand(id);
	treeCtrl->SelectItem(id);
	treeCtrl->EditLabel(id);

	NodeOperation* nodeOperation = new NodeOperation();
	nodeOperation->weakNode = node;
	nodeOperation->scenePage = this;

	UndoManager* undoManager = editor->getUndoManager();
	undoManager->registerOperation(nodeOperation);
}

//-----------------------------------//

void ScenePage::onButtonNodeDelete(wxCommandEvent&)
{	
	wxTreeItemId id = treeCtrl->GetSelection();
	
	NodePtr node = getNodeFromTreeId(id);

	if( !node )
		return;

	assert( node != nullptr );
	assert( nodeIds[node] == id );

	nodeIds.erase(node);
	treeCtrl->Delete(id);
	weakScene.lock()->remove( node );

	editor->redrawView();
}

//-----------------------------------//

void ScenePage::onButtonNodeDeleteUpdate(wxUpdateUIEvent& event)
{
	ScenePtr scene = weakScene.lock();

	if( !scene )
		return;

	bool empty = scene->getChildren().empty();
	
	event.Enable( !empty );
}

//-----------------------------------//

void ScenePage::onItemMenu(wxTreeEvent& event)
{
	menuItemId = event.GetItem();
	const NodePtr& node = getNodeFromTreeId( menuItemId );

	wxMenu menu("Scene node");
	currentMenu = &menu;

	if( !node )
		return;

	menu.AppendCheckItem(ID_MenuSceneNodeVisible, "&Visible");
	menu.Check(ID_MenuSceneNodeVisible, node->isVisible() );

	const std::vector< GeometryPtr >& geo = node->getGeometry();
	
	if( !geo.empty() )
	{
		const std::vector< RenderablePtr >& rend = geo.front()->getRenderables();
		if( !rend.empty() )
		{
			bool state = (rend.front()->getPolygonMode() != PolygonMode::Solid);
			menu.AppendCheckItem(ID_MenuSceneNodeWireframe, "&Wireframe");
			menu.Check(ID_MenuSceneNodeWireframe, state );
		}
	}

	menu.Append(ID_MenuSceneNodeDelete, "&Delete");
	menu.Append(ID_MenuSceneNodeDuplicate, "Duplicate");

	menu.AppendSeparator();

	foreach( ComponentEntry& c, components )
	{
		const Type& type = c.type;
		wxMenuItem* item = menu.Append(wxID_ANY, type.getName());
		item->SetBitmap( bitmaps[&type], false );
	}

	wxPoint clientpt = event.GetPoint();
	PopupMenu(&menu, clientpt);
}

//-----------------------------------//	

template<typename T>
T* cloneObject(T* object)
{
	const Class& type = object->getInstanceType();
	T* newObject = (T*) type.createInstance();

	foreach( const FieldsPair& p, type.getFields() )
	{
		Field* field = p.second;
		const Type& field_type = field->type;
		
		if( field->isPointer() )
		{
			field->set<boost::intrusive_ptr<Resource>>(
				newObject, field->get<boost::intrusive_ptr<Resource>>(object));
		}

		if( field_type.isPrimitive() )
		{
			const Primitive& type = (Primitive&) field_type;

			if( type.isBool() )
			{
				field->set<bool>(newObject, field->get<bool>(object));
			}
			//-----------------------------------//
			else if( type.isInteger() )
			{
				field->set<int>(newObject, field->get<int>(object));
			}
			//-----------------------------------//
			else if( type.isFloat() )
			{
				field->set<float>(newObject, field->get<float>(object));
			}
			//-----------------------------------//
			else if( type.isString() )
			{
				field->set<std::string>(newObject, field->get<std::string>(object));
			}
			//-----------------------------------//
			else if( type.isColor() )
			{
				field->set<Color>(newObject, field->get<Color>(object));
			}
			//-----------------------------------//
			else if( type.isVector3() )
			{
				field->set<Vector3>(newObject, field->get<Vector3>(object));
			}
		}
	}

	return newObject;
}

//-----------------------------------//

void ScenePage::onMenuSelected( wxCommandEvent& event )
{
	int id = event.GetId();

	const NodePtr& node = getNodeFromTreeId( menuItemId );
	
	if( !node )
		return;

	if( id == ID_MenuSceneNodeVisible )
	{		
		node->setVisible( !node->isVisible() );
	}
	//-----------------------------------//
	else if( id == ID_MenuSceneNodeDuplicate )
	{
		ScenePtr scene = weakScene.lock();
	
		NodePtr newNode( cloneObject<Node>(node.get()) );

		foreach( const ComponentMapPair& p, node->getComponents() )
		{
			ComponentPtr component = p.second;
			ComponentPtr newComponent( cloneObject<Component>(component.get()) );
			newNode->addComponent(newComponent);
		}
		
		scene->add(newNode);
	}
	//-----------------------------------//
	else if( id == ID_MenuSceneNodeWireframe )
	{
		const NodePtr& node = getNodeFromTreeId( menuItemId );
		
		if( !node )
			return;

		PolygonMode::Enum mode = event.IsChecked() ?
			PolygonMode::Wireframe : PolygonMode::Solid;

		foreach( const GeometryPtr& geo, node->getGeometry() )
		{
			foreach( const RenderablePtr& rend, geo->getRenderables() )
			{
				rend->setPolygonMode( mode );
			}
		}
	}
	else if( id == ID_MenuSceneNodeDelete )
	{
		std::string str = (std::string) treeCtrl->GetItemText(menuItemId);

		TypeRegistry& typeRegistry = TypeRegistry::getInstance();
		const Class* type = (Class*) typeRegistry.getType(str);
		assert( type != nullptr );

		if( !type->inherits<Component>() )
			return;

		const ComponentMap& components = node->getComponents();
		ComponentMap::const_iterator it = components.find(type);

		if( it == components.end() )
			return;

		ComponentPtr component = it->second;
		node->removeComponent(component);
	}
	//-----------------------------------//
	else
	{
		onComponentAdd( event );
	}

	editor->redrawView();
}

//-----------------------------------//

void ScenePage::onComponentAdd(wxCommandEvent& event )
{
	const NodePtr& node = getNodeFromTreeId( menuItemId );
	
	int id = event.GetId();
	const wxString& name =  currentMenu->GetLabelText(id); 
	
	TypeRegistry& registry = TypeRegistry::getInstance();
	const Type* type = registry.getType( (std::string) name );
	assert( type != nullptr );

	ComponentPtr component;

	if( type->is<Model>() )
	{
		wxFileDialog fd( this, wxFileSelectorPromptStr,
			wxEmptyString, wxEmptyString, "Mesh files (*.ms3d)|*.ms3d",
			wxFD_DEFAULT_STYLE|wxFD_FILE_MUST_EXIST );

		if( fd.ShowModal() == wxID_OK )
		{
			std::string filename( fd.GetPath() );
			std::vector<std::string> elems = String::split( filename, '\\' );

			ResourceManager* rm = engine->getResourceManager();
			MeshPtr mesh = rm->loadResource<Mesh>( elems.back() );
			
			component.reset( new Model(mesh) );
		}
	}
	else
	{
		Class& classType = (Class&) *type;
		component.reset( (Component*) classType.createInstance() );
	}

	if( node->addComponent(component) )
		addComponent(menuItemId, component);
}

//-----------------------------------//

void ScenePage::onNodeAdded( const GroupEvent& event )
{
	const NodePtr& node = event.node;

	if( node->getTag(EditorTags::EditorOnly) )
		return;
	
	addNode(rootId, node);
}

//-----------------------------------//

void ScenePage::onNodeRemoved( const GroupEvent& event )
{
	//const std::string& type = component.second->getType();
	//AppendItem( id, type, icons[type] );
	Events* events = editor->getEventManager();
	events->onNodeUnselect( event.node );
}

//-----------------------------------//

void ScenePage::onLabelEditBegin( wxTreeEvent& event )
{
	wxTreeItemId item = event.GetItem();
	NodePtr node = getNodeFromTreeId( item );
	
	if( !node )
	{
		event.Veto();
	}
}

//-----------------------------------//

void ScenePage::onLabelEditEnd( wxTreeEvent& event )
{
	wxTreeItemId item = event.GetItem();
	const wxString& label = event.GetLabel();

	if( event.IsEditCancelled() )
	{
		event.Veto();
		return;
	}

	#pragma TODO("Test for duplicate scene names")

	if( label.empty() )
	{
		event.Veto();
		return;
	}

	NodePtr node = getNodeFromTreeId( item );
	assert( node != nullptr );

	node->setName( std::string( label.c_str() ) );
}

//-----------------------------------//

void ScenePage::onDragBegin( wxTreeEvent& event )
{
	//event.Allow();

	// Save the dragged original item, for later processing...
	dragItemId = event.GetItem();
}

//-----------------------------------//

void ScenePage::onDragEnd( wxTreeEvent& event )
{
	// Get the drop point location and do an hit test to check if
	// it was was done in a valid tree item id location.
	
	wxPoint drag_point = event.GetPoint();
	wxTreeItemId drag_id = treeCtrl->HitTest( drag_point );

	// If the drop was not done in a valid tree location, 
	// then we've got nothing to do here, move along...
	if( !drag_id.IsOk() )
		return;

	const NodePtr& src = getNodeFromTreeId( dragItemId );
	const NodePtr& dest = getNodeFromTreeId( drag_id );

	if( src && dest ) // It is a node, just re-parent it.
	{
		src->setParent( dest );
	}
	else // It was probably a component, so handle it.
	{
		#pragma TODO("Move component from one node to another")
	}

	//DeleteChildren( root );
	//updateScene( root, scene );
}

//-----------------------------------//

void ScenePage::onContextMenu( wxContextMenuEvent& event )
{
	wxPoint clientpt = event.GetPosition();

	wxMenu menu("Scene");
	//menu.Append(ID_MenuSceneNodeDelete, "&Delete...");

	PopupMenu(&menu, clientpt);
}

//-----------------------------------//

} } // end namespaces