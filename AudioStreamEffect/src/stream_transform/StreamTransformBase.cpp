#include "StreamTransformBase.h"
#include "imgui.h"

bool StreamTransform::Draw()
{
    bool open(true);
    if (ImGui::Begin((std::string("Transform: ") + IGetTransformName() + "###transform").c_str(), &open))
    {
        ITransformDraw();
    }
    ImGui::End();
    return open;
}
