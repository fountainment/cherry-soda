#include <CherrySoda/Components/Graphics/GraphicsComponent.h>

namespace cherrysoda {

const Math::Mat4 GraphicsComponent::GetTransformMatrix() const
{
	return Math_Translate(
		Math_Scale(
			Math_Rotate(
				Math_Translate(
					Math_Identity<Math::Mat4>(),
				RenderPosition()),
			ZRotation(), Vec3_ZUp),
		Scale()),
	-Origin());
}

} // namespace cherrysoda
