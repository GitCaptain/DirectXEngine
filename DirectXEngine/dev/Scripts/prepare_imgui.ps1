
$imguiDir = "$PSScriptRoot\..\..\..\imgui"
$incDstDir = "$PSScriptRoot\..\..\ThirdParty\include\ImGUI"
$srcDstDir = "$PSScriptRoot\..\..\ThirdParty\sources\ImGUI"
$exclude = @('imgui_demo.cpp')

mkdir -Force "$incDstDir"
mkdir -Force "$srcDstDir"
Copy-Item "$imGuiDir\*.h" "$incDstDir"
Copy-Item "$imGuiDir\*.cpp" "$srcDstDir" -Exclude $exclude
Copy-Item "$imGuiDir\backends\imgui_impl_win32.h" "$incDstDir"
Copy-Item "$imGuiDir\backends\imgui_impl_win32.cpp" "$srcDstDir"
Copy-Item "$imGuiDir\backends\imgui_impl_dx11.h" "$incDstDir"
Copy-Item "$imGuiDir\backends\imgui_impl_dx11.cpp" "$srcDstDir"
