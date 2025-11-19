#include <EditorApp.h>

int main() {
	EditorApp* editorApp = new EditorApp();
	editorApp->InitEditorApp(1000u, 800u);
	editorApp->RunApp();
	
}
