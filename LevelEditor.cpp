
#include "LevelEditor.h"
#include "Vector.h"

// Deal with statix
Vector LevelEditor::vCursorPos = Vector(0,0,0);
int LevelEditor::iEditorState = EDITOR_STATE_WAITING;
//int LevelEditor::iSelectState = EDITOR_SELECT_NONE;
int LevelEditor::iPlaceObjectType  = BASETYPE_TREE;
Base *LevelEditor::pLastObjectPlaced = NULL;
Base *LevelEditor::pCopyObject = NULL;
Base *LevelEditor::pSelectedObject = NULL;
float *LevelEditor::pfEdit = NULL;

