#include "StdAfx.h"
#include "GlbGlobeAutoTransform.h"
#include "GlbGlobeView.h"
#include "osg/CullStack"

using namespace GlbGlobe;

GlbGlobe::GlbGlobeAutoTransform::GlbGlobeAutoTransform( CGlbGlobe *globe /*= NULL*/ )
{
	mpr_globe = globe;
}

GlbGlobeAutoTransform::~GlbGlobeAutoTransform(void)
{
}

// ²Î¿¼ osg::AutoTransform::accept()
void GlbGlobeAutoTransform::accept( NodeVisitor& nv )
{
	if (nv.validNodeMask(*this))
	{
		// if app traversal update the frame count.
		if (nv.getVisitorType()==NodeVisitor::UPDATE_VISITOR)
		{
		}
		else
			if (nv.getVisitorType()==NodeVisitor::CULL_VISITOR)
			{

				CullStack* cs = dynamic_cast<CullStack*>(&nv);
				if (cs)
				{

					Viewport::value_type width = _previousWidth;
					Viewport::value_type height = _previousHeight;

					osg::Viewport* viewport = cs->getViewport();
					if (viewport)
					{
						width = viewport->width();
						height = viewport->height();
					}

					osg::Vec3d eyePoint = cs->getEyeLocal();
					osg::Vec3d localUp = cs->getUpLocal();
					osg::Vec3d position = getPosition();

					const osg::Matrix& projection = *(cs->getProjectionMatrix());

					bool doUpdate = _firstTimeToInitEyePoint;
					if (!_firstTimeToInitEyePoint)
					{
						osg::Vec3d dv = _previousEyePoint-eyePoint;
						if (dv.length2()>getAutoUpdateEyeMovementTolerance()*(eyePoint-getPosition()).length2())
						{
							doUpdate = true;
						}
						osg::Vec3d dupv = _previousLocalUp-localUp;
						// rotating the camera only affects ROTATE_TO_*
						if (_autoRotateMode &&
							dupv.length2()>getAutoUpdateEyeMovementTolerance())
						{
							doUpdate = true;
						}
						else if (width!=_previousWidth || height!=_previousHeight)
						{
							doUpdate = true;
						}
						else if (projection != _previousProjection)
						{
							doUpdate = true;
						}
						else if (position != _previousPosition)
						{
							doUpdate = true;
						}
					}
					_firstTimeToInitEyePoint = false;

					if (doUpdate)
					{

						if (getAutoScaleToScreen())
						{
							double size = 1.0/cs->pixelSize(getPosition(),0.48f);

							if (_autoScaleTransitionWidthRatio>0.0)
							{
								if (_minimumScale>0.0)
								{
									double j = _minimumScale;
									double i = (_maximumScale<DBL_MAX) ?
										_minimumScale+(_maximumScale-_minimumScale)*_autoScaleTransitionWidthRatio :
									_minimumScale*(1.0+_autoScaleTransitionWidthRatio);
									double c = 1.0/(4.0*(i-j));
									double b = 1.0 - 2.0*c*i;
									double a = j + b*b / (4.0*c);
									double k = -b / (2.0*c);

									if (size<k) size = _minimumScale;
									else if (size<i) size = a + b*size + c*(size*size);
								}

								if (_maximumScale<DBL_MAX)
								{
									double n = _maximumScale;
									double m = (_minimumScale>0.0) ?
										_maximumScale+(_minimumScale-_maximumScale)*_autoScaleTransitionWidthRatio :
									_maximumScale*(1.0-_autoScaleTransitionWidthRatio);
									double c = 1.0 / (4.0*(m-n));
									double b = 1.0 - 2.0*c*m;
									double a = n + b*b/(4.0*c);
									double p = -b / (2.0*c);

									if (size>p) size = _maximumScale;
									else if (size>m) size = a + b*size + c*(size*size);
								}
							}

							setScale(size);
						}

						if (_autoRotateMode==ROTATE_TO_SCREEN)
						{
							osg::Vec3d translation;
							osg::Quat rotation;
							osg::Vec3d scale;
							osg::Quat so;

							cs->getModelViewMatrix()->decompose( translation, rotation, scale, so );

							setRotation(rotation.inverse());
						}
						else if (_autoRotateMode==ROTATE_TO_CAMERA)
						{
							osg::Vec3d PosToEye = _position - eyePoint;
							osg::Matrix lookto = osg::Matrix::lookAt(
								osg::Vec3d(0,0,0), PosToEye, localUp);
							Quat q;
							q.set(osg::Matrix::inverse(lookto));
							setRotation(q);
						}
						else if (_autoRotateMode==ROTATE_TO_AXIS)
						{
							Quat q;
							switch(_cachedMode)
							{
							case(AXIAL_ROT_Z_AXIS):
								{
									q = Quat(0,0,0,1);
									break;
								}
							case(AXIAL_ROT_Y_AXIS):
								{
									break;
								}
							case(AXIAL_ROT_X_AXIS):
								{
									glbDouble pitch = 0.0;
									if(mpr_globe)
									{
										pitch = mpr_globe->GetView()->GetPitch();
									}
									q.makeRotate(osg::DegreesToRadians(pitch + 90),osg::Vec3d(1,0,0));
									break;
								}
							case(ROTATE_TO_AXIS): // need to implement
								{
									break;
								}
							}
							setRotation(q);
						}

						_previousEyePoint = eyePoint;
						_previousLocalUp = localUp;
						_previousWidth = width;
						_previousHeight = height;
						_previousProjection = projection;
						_previousPosition = position;
						_matrixDirty = true;
					}

				}
			}

			// now do the proper accept
			Transform::accept(nv);
	}
}