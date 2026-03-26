#include <iostream>
#include <string>

#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridReader.h>

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Usage: tet_view <mesh.vtu>\n";
    return 1;
  }

  const std::string path = argv[1];

  vtkNew<vtkXMLUnstructuredGridReader> reader;
  reader->SetFileName(path.c_str());
  reader->Update();

  vtkUnstructuredGrid* grid = reader->GetOutput();
  if (grid == nullptr || grid->GetNumberOfCells() == 0) {
    std::cerr << "Failed to read VTU or mesh is empty: " << path << '\n';
    return 2;
  }

  vtkNew<vtkDataSetMapper> mapper;
  mapper->SetInputData(grid);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetOpacity(0.35);
  actor->GetProperty()->SetRepresentationToSurface();

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
  renderer->AddActor(actor);

  vtkNew<vtkRenderWindow> window;
  window->SetSize(1280, 800);
  window->AddRenderer(renderer);
  window->SetWindowName("Tet mesh viewer");

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(window);

  window->Render();
  interactor->Start();

  return 0;
}
