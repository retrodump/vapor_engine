/************************************************************************
*
* vapor3D Editor © (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#pragma once

#include "Plugin.h"

namespace vapor { namespace editor {

//-----------------------------------//

class ProjectPlugin : public Plugin
{
public:

	ProjectPlugin( EditorFrame* frame );

	// Gets metadata about this plugin.
	virtual PluginMetadata getMetadata();

	// Plugin enable callback.
	virtual void onPluginEnable();

	// Plugin disable callback.
	virtual void onPluginDisable();

	// Handles new button click.
	void onNewButtonClick(wxCommandEvent& event);

	// Handles open button click.
	void onOpenButtonClick(wxCommandEvent& event);

	// Handles save button click.
	void onSaveButtonClick(wxCommandEvent& event);

	// Updates the save button UI state.
	void onSaveButtonUpdateUI(wxUpdateUIEvent& event);

	// New toolbar button.
	wxAuiToolBarItem* newButton;

	// Open toolbar button.
	wxAuiToolBarItem* openButton;

	// Save toolbar button.
	wxAuiToolBarItem* saveButton;

protected:

	// Handles undo/redo events.
	void onUndoRedoEvent();

	// Switches the scene.
	void switchScene(const ScenePtr& scene);

	// Asks to save unsaved changes.
	bool askSaveChanges();

	// Saves the current scene.
	bool saveScene();

	// Keeps track if there are unsaved changes.
	bool unsavedChanges;
};

//-----------------------------------//

} } // end namespaces