#include "StdAfx.h"
#include "GlbGlobeMarkerLabelSymbol.h"
#include "osg/Billboard"
#include "osg/Texture2D"
#include "osgDB/ReadFile"
#include "osg/BlendColor"
#include <osgDB/WriteFile>
#include "GlbString.h"

#include "agg_renderer_scanline.h"
#include "agg_scanline_u.h"
#include "agg_scanline_p.h"
#include "platform/win32/agg_win32_bmp.h"
#include "agg_pixfmt_rgb.h"
#include "agg_span_allocator.h"
#include "agg_span_interpolator_linear.h"
#include "agg_span_gradient_alpha.h"
#include "agg_span_image_filter_rgb.h"
#include "agg_span_converter.h"
#include "agg_basics.h"

#include "GlbPoint.h"
#include "CGlbGlobe.h"
#include "GlbGlobeView.h"

#include "platform/agg_platform_support.h"
#include "util/agg_color_conv.h"
#include "util/agg_color_conv_rgb8.h"
#include <agg_conv_dash.h>

#include "osg/BlendFunc"
#include "osg/TexEnv"
#include "osg/Material"

#include "Drawer2D.h"
#include "agg_span_image_filter_rgba.h"
#include "agg_image_accessors.h"
#include "agg_renderer_base.h"
#include "agg_pixfmt_rgba.h"

#include <osg/Depth>
#include <osg/ImageStream>
#include "GlbLog.h"

using namespace GlbGlobe;
#define WIDTHBYTES(bites) (((bites) + 31) / 32 * 4);

CGlbGlobeMarkerLabelSymbol::CGlbGlobeMarkerLabelSymbol(void)
{
	cglbExtent = NULL;
	//mpr_clampImage = NULL;
	mpr_isComputeOutline = false;
	mpr_font = new agg::font_engine_win32_tt_int16(::GetDC(::GetActiveWindow()));
	mpr_textFont = L"宋体";
	mpr_font->flip_y(true); //上下翻转
	mpr_font->hinting(true); //字体修正
	mpr_font->create_font("宋体",agg::glyph_ren_agg_gray8);
	mpr_isDirectDraw = false;
}

CGlbGlobeMarkerLabelSymbol::~CGlbGlobeMarkerLabelSymbol(void)
{
	if(mpr_font)
		delete mpr_font;
	mpr_font = NULL;
}

osg::Node * CGlbGlobeMarkerLabelSymbol::Draw( CGlbGlobeRObject *obj,IGlbGeometry *geo )
{
	glbref_ptr<GlbRenderInfo> renderInfo = obj->GetRenderInfo();
	GlbMarkerLabelSymbolInfo *labelInfo = 
		static_cast<GlbMarkerLabelSymbolInfo *>(renderInfo.get());
	if(labelInfo == NULL)
		return NULL;
	CGlbFeature *feature = obj->GetFeature();
	CGlbGlobe *globe = obj->GetGlobe();
	if(NULL == globe) return NULL;

	//osg::Group *group = new osg::Group;
	osg::MatrixTransform *group = new osg::MatrixTransform;
	int sizeWidth = 0,sizeHeight = 0;
	osg::ref_ptr<osg::Image> tempImage = NULL;
	if(labelInfo->imageInfo && labelInfo->imageInfo->dataSource)
	{
		CGlbWString datalocate = labelInfo->imageInfo->dataSource->GetValue(feature);
		glbInt32 index = datalocate.find_first_of(L'.');
		if(index == 0)
		{// 处理当前执行文件的相对路径情况 ./AAA/....
			CGlbWString execDir = CGlbPath::GetExecDir();
			datalocate = execDir + datalocate.substr(1,datalocate.size());
		}
		tempImage = osgDB::readImageFile(datalocate.ToString());
		if(tempImage)
		{
			sizeWidth = tempImage->s();
			sizeHeight = tempImage->t();

			osg::ImageStream* imagestream = dynamic_cast<osg::ImageStream*>(tempImage.get());
			if (imagestream) 
				imagestream->play();
		}
		else if (datalocate.length() > 0)
		{
			GlbLogWOutput(GLB_LOGTYPE_ERR,L"读取标注图像文件(%s)失败. \r\n",datalocate.c_str());
		}				
	}

	glbInt32 maxSize = -1;
	if(labelInfo->imageInfo && labelInfo->imageInfo->maxSize)
		maxSize = labelInfo->imageInfo->maxSize->GetValue(feature);
	double pixelRatioW = 1.0;
	double pixelRatioH = 1.0;

	if(tempImage)
	{
		if(maxSize >= 0 && (maxSize != sizeWidth || maxSize != sizeHeight))
		{
			if(sizeWidth >= sizeHeight)
			{
				pixelRatioW = (double)maxSize / sizeWidth;
				pixelRatioH = (double)maxSize / sizeWidth;
			}
			else
			{
				pixelRatioW = (double)maxSize / sizeHeight;
				pixelRatioH = (double)maxSize / sizeHeight;
			}
		}

		tempImage->scaleImage(sizeWidth * pixelRatioW,sizeHeight * pixelRatioH,tempImage->r()/*,dataType*/);
		sizeWidth = tempImage->s();
		sizeHeight = tempImage->t();
	}

	glbBool isEmpty = true;
	glbInt32 textSize = 0;
	std::vector<CGlbWString> wsRenderContainer;
	glbDouble bytes = 0.0;//记录对齐方式参考字节数

	CGlbWString content = L"";
	if(labelInfo->textInfo && labelInfo->textInfo->size)
		textSize = labelInfo->textInfo->size->GetValue(feature);
	if(labelInfo->textInfo && labelInfo->textInfo->content)
		content = labelInfo->textInfo->content->GetValue(feature);

	glbInt32 textCount = content.size();
	if(textCount != 0)
		isEmpty = false;

	//DealMultilineAlignText函数不仅处理了多行问题，还会判断你传进的字符串是否是多行
	//由于解决后期发现的设置加粗、倾斜会出问题，
	//把设置这两步的代码写到这里，具体原因没分析出来为什么
	if(!isEmpty && labelInfo->textInfo)
		wsRenderContainer = DealMultilineAlignText(labelInfo->textInfo,feature,bytes);

	GlbGlobeLabelRelationEnum labelRelation = GLB_LABEL_ON_IMAGE;
	if(labelInfo->labelImageRelationInfo->relation)
		labelRelation = (GlbGlobeLabelRelationEnum)labelInfo->labelImageRelationInfo->relation->GetValue(feature);

	GlbGlobeLabelAlignTypeEnum labelAlign = GLB_LABELALG_LEFTBOTTOM;
	if(labelInfo->labelImageRelationInfo && labelInfo->labelImageRelationInfo->align)
		labelAlign = (GlbGlobeLabelAlignTypeEnum)labelInfo->labelImageRelationInfo->align->GetValue(feature);
	glbDouble x = 0.0,y = 0.0;
	if(isEmpty == false)
	{
		CGlbWString textFont = L"宋体";
		glbBool isItalic = false;
		if(labelInfo->textInfo && labelInfo->textInfo->font)
			textFont = labelInfo->textInfo->font->GetValue(feature);//字体
		if(labelInfo->textInfo && labelInfo->textInfo->isItalic)
			isItalic = labelInfo->textInfo->isItalic->GetValue(feature);
		DealComputeBufferSize(wsRenderContainer.size(),textSize,textFont,
			content,bytes,labelRelation,labelAlign,isItalic,tempImage,sizeWidth,sizeHeight,x,y,maxSize);
	}

	if(sizeWidth == 0 || sizeHeight == 0)
		return NULL;

	glbBool isLimit = false;
	glbDouble ratio = 1.0;
	if(labelInfo->geoInfo->isLimit)
		isLimit = labelInfo->geoInfo->isLimit->GetValue(feature);
	if(labelInfo->geoInfo->ratio)
		ratio = labelInfo->geoInfo->ratio->GetValue(feature);

	if(mpr_isComputeOutline)
	{
		mpr_halfWidth = sizeWidth * ratio / 2;
		mpr_halfHeight = sizeHeight * ratio/ 2;
		return NULL;
	}

	int bits = 0;
	unsigned char * buffer  = NULL;
	bits = WIDTHBYTES(sizeWidth * 32);
	bits *= sizeHeight;
	buffer = new unsigned char[bits];
	memset(buffer,0,sizeof(unsigned char) * sizeWidth * sizeHeight * 4);
	agg::rendering_buffer rbuf(buffer,sizeWidth,sizeHeight,-sizeWidth * 4);

	agg::pixfmt_rgba32 pixf(rbuf);
	renb_type renb(pixf);

	glbInt32 textbackColor = 0xFFFFFFFF;
	glbInt32 textbackOpacity = 100;
	if(labelInfo->textInfo->backColor)
		textbackColor = labelInfo->textInfo->backColor->GetValue(feature);
	if(labelInfo->textInfo->backOpacity)
		textbackOpacity = labelInfo->textInfo->backOpacity->GetValue(feature);

	if(0xFFFFFFFF != textbackColor && textbackOpacity != 0)
	{
		osg::Vec4 backColor = GetColor(textbackColor);
		renb.clear(agg::rgba(backColor.x(), backColor.y(), backColor.z(),textbackOpacity / 100.0));
	}

	//ceshi
	//renb.clear(agg::rgba(1.0,1.0,1.0,1.0));

	if(labelInfo && (labelInfo->imageInfo || labelInfo->textInfo))
		DealRenderImageAndText(isEmpty,wsRenderContainer,content,x,bytes,
		textSize,tempImage,labelInfo->imageInfo,labelInfo->textInfo,
		labelRelation,labelAlign,sizeWidth,sizeHeight,renb,buffer,feature);

	//通过agg处理过的buff创建
	ref_ptr<osg::Image> image = new osg::Image;
	image->allocateImage(sizeWidth,sizeHeight,1,GL_RGBA, GL_UNSIGNED_BYTE);
	unsigned char *data = image->data();
	memcpy(data,buffer,sizeof(unsigned char) * sizeWidth* sizeHeight * 4);

	if(buffer)
	{
		delete[] buffer;
		buffer = NULL;
	}

	if(obj->GetAltitudeMode() == GLB_ALTITUDEMODE_ONTERRAIN)
	{
		obj->SetCustomData(image.get());
		return NULL;
	}

	//osgDB::writeImageFile(*image,"D:/testNotClamp2.png");

	ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	if (image)
	{
		texture->setImage(image);
		stateset->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
	}

	osg::ref_ptr<GlbGlobeAutoTransform> autoTransform = new GlbGlobeAutoTransform(globe);
	GlbGlobeBillboardModeEnum mode = GLB_BILLBOARD_SCREEN;
	if(labelInfo->mode)
	{
		mode = (GlbGlobeBillboardModeEnum)labelInfo->mode->GetValue(feature);
		if (mode == GLB_BILLBOARD_SCREEN)//屏幕
			autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
		else if (mode == GLB_BILLBOARD_AXIS)//轴向
		{
			autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
			autoTransform->setAxis(osg::Vec3(0.0f,0.0f,1.0f));
			autoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
		}
		else if(mode == GLB_BILLBOARD_AUTOSLOPE)//自动旋转
		{
			autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
			autoTransform->setAxis(osg::Vec3(1.0f,0.0f,0.0f));
			autoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
		}
	}

	GlbGlobeLabelAlignTypeEnum positionAlign = GLB_LABELALG_LEFTBOTTOM;
	if(labelInfo->labelAlign)
		positionAlign = (GlbGlobeLabelAlignTypeEnum)labelInfo->labelAlign->GetValue(feature);

	osg::ref_ptr<osg::Geometry> labelGeom = NULL;
	osg::Vec3 corner(0.0,0.0,0.0);////GLB_LABELALG_LEFTBOTTOM
	osg::Vec3 width,height;

	if (!isLimit)
	{
		width = osg::Vec3(sizeWidth * ratio,0.0,0.0);
		height = osg::Vec3(0.0,sizeHeight * ratio,0.0);
		corner = ComputeCornerByAlign(sizeWidth,sizeHeight,ratio,positionAlign);
		labelGeom = osg::createTexturedQuadGeometry(corner,width,height);
	}
	else
	{
		width = osg::Vec3(sizeWidth/* * ratio*/,0.0,0.0);
		height = osg::Vec3(0.0,sizeHeight/* * ratio*/,0.0);
		corner = ComputeCornerByAlign(sizeWidth,sizeHeight,1.0,positionAlign);
		labelGeom = osg::createTexturedQuadGeometry(corner,width,height);
		autoTransform->setAutoScaleToScreen(true);
		autoTransform->setMinimumScale(0.0);
		//if(-1 == maxSize)
		//	autoTransform->setMaximumScale(DBL_MAX);
		//else
		autoTransform->setMaximumScale(/*maxSize * */ratio);
	}

	//osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material *>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
	//if(material == NULL)
	//	material = new osg::Material;
	//material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(1,1,1,1));
	//material->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(1,1,1,1));
	//material->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(1,1,1,1));
	//material->setSpecular(osg::Material::FRONT_AND_BACK,osg::Vec4(1,1,1,1));
	//material->setAlpha(osg::Material::FRONT_AND_BACK,1.0);
	//stateset->setAttribute(material,osg::StateAttribute::ON);
	//透明度
	osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::ONE/*SRC_ALPHA*/,osg::BlendFunc::ONE_MINUS_SRC_ALPHA);	//设置混合方程
	stateset->setAttributeAndModes(pBlendFun.get(),osg::StateAttribute::ON);
	stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
	stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON);
	stateset->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
	stateset->setMode( GL_NORMALIZE, osg::StateAttribute::ON );

	osg::ref_ptr<osg::Depth> depth = dynamic_cast<osg::Depth *>(stateset->getAttribute(osg::StateAttribute::DEPTH));
	if(!depth.valid())
	{
		depth = new osg::Depth;
		depth->setWriteMask(false);
		stateset->setAttributeAndModes(depth.get(), osg::StateAttribute::ON);
	}
	else
	{
		if(depth->getWriteMask())
			depth->setWriteMask(false);
	}

	osg::ref_ptr<osg::TexEnv> texenv=new osg::TexEnv;
	texenv->setMode(osg::TexEnv::MODULATE);
	//设置纹理环境
	stateset->setTextureAttribute(0,texenv.get());
	labelGeom->setStateSet(stateset);

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::Geode> outGeode = new osg::Geode;
	osg::ref_ptr<GlbGlobeAutoTransform> outAutoTransform = new GlbGlobeAutoTransform(globe);
	//内框
	osg::ref_ptr<osg::Geometry> lineGeometry = CreateOutlineQuad(corner,width,height);
	geode->addDrawable(labelGeom);
	//geode->addDrawable(lineGeometry);
	autoTransform->addChild(geode);
	group->addChild(autoTransform);
	//添加外框
	if(isLimit)
	{
		osg::ref_ptr<osg::Geometry> outlineGeometry = CreateOutlineQuad(corner,width,height);
		if(NULL == outlineGeometry) return group;
		//outGeode->addDrawable(outlineGeometry);
		outAutoTransform->addChild(outGeode);
		if(GLB_BILLBOARD_SCREEN == mode)
			outAutoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
		else if(GLB_BILLBOARD_AXIS == mode)
		{
			outAutoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
			outAutoTransform->setAxis(osg::Vec3(0.0f,0.0f,1.0f));
			outAutoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
		}
		else if(GLB_BILLBOARD_AUTOSLOPE == mode)
		{
			outAutoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
			outAutoTransform->setAxis(osg::Vec3(1.0f,0.0f,0.0f));
			outAutoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
		}
		outAutoTransform->setAutoScaleToScreen(true);
		outAutoTransform->setMinimumScale(DBL_MAX);
		outAutoTransform->setMaximumScale(/*maxSize * */ratio);
		group->addChild(outAutoTransform);
	}

	glbDouble xOffset = 0.0,yOffset = 0.0,zOffset = 0.0;
	if(labelInfo->xOffset && labelInfo->yOffset && labelInfo->zOffset)
	{
		xOffset = labelInfo->xOffset->GetValue(feature);
		yOffset = labelInfo->yOffset->GetValue(feature);
		zOffset = labelInfo->zOffset->GetValue(feature);
		osg::Matrix m;
		m.makeTranslate(xOffset,yOffset,zOffset);
		group->setMatrix(m);
	}

	return group;
}

void CGlbGlobeMarkerLabelSymbol::DealImage(osg::Vec2d leftUp,
	osg::Vec2d rightDown,GlbGlobeLabelRelationEnum labelRelation,osg::Image *image,
	renb_type &renb/*RGBA*/,unsigned char * buffer,GlbRenderImageInfo *imageInfo,CGlbFeature *feature)
{
	if(imageInfo == NULL || image == NULL)
		return;

	glbInt32 imageColor = 0xFFFFFFFF;
	glbInt32 imageOpacity = 100;
	if(imageInfo->color)
		imageColor = imageInfo->color->GetValue(feature);
	if(imageInfo->opacity)
		imageOpacity = imageInfo->opacity->GetValue(feature);

	//changshi
	unsigned char* pdata = NULL;
	agg::int8u apha(imageOpacity * 255 / 100.0);//图片透明度
	typedef agg::renderer_base<agg::pixfmt_rgba32> renderer_base_type;
	typedef agg::renderer_scanline_aa_solid<renderer_base_type> renderer_scanline_type;
	renderer_scanline_type rensl(renb);
	agg::rasterizer_scanline_aa<> ras;
	agg::scanline_u8 sl;

	if(image)
	{
		//stride应该是width的倍数
		agg::rendering_buffer rbuf_img(NULL,0,0,0);
		GLenum dataType = (GLenum)image->getPixelFormat();

		if (dataType == GL_RGB)
		{
			rbuf_img = agg::rendering_buffer(image->data(),
				image->s(),image->t(), - 3 * image->s());
		}
		else if (dataType == GL_RGBA)
		{
			int width = image->s();
			int height = image->t();
			int cnt = image->s() * image->t() * 3;
			pdata = new unsigned char[cnt];
			unsigned char *data = image->data();
			for(int i = 0; i <image->t();i++)
				for(int j =0; j < image->s(); j++)
				{
					unsigned char* r = data + ( i * image->s() + j) * 4;
					unsigned char* g = data + ( i * image->s() + j) * 4 + 1;
					unsigned char* b = data + ( i * image->s() + j) * 4 + 2;
					unsigned char* a = data + ( i * image->s() + j) * 4 + 3;
					//if(*a != 255)
					//	int a = 1;

					pdata[(i * image->s() + j) * 3] = *r;
					pdata[(i * image->s() + j) * 3 + 1] = *g;
					pdata[(i * image->s() + j) * 3 + 2] = *b;
				}
				rbuf_img = agg::rendering_buffer(pdata,image->s(),image->t(), - 3 * image->s());
		}

		agg::pixfmt_rgba32 pixf_img(rbuf_img);
		typedef agg::span_allocator<agg::rgba8> span_allocator_type;
		span_allocator_type span_alloc;
		typedef agg::span_interpolator_linear<> interpolator_type;
		agg::trans_affine img_mtx;
		interpolator_type ip_img(img_mtx);

		typedef std::vector<agg::int8u> alphaF_type;
		alphaF_type alphaF(image->s());
		for (int i = 0; i < image->s();i++)
			alphaF[i] = apha;

		//if (labelRelation == GLB_LABEL_SURROUND_IMAGE)
		img_mtx.translate(leftUp.x(),leftUp.y());
		img_mtx.invert();

		agg::trans_affine alpha_mtx;
		interpolator_type ip_alpha(alpha_mtx);
		typedef agg::gradient_x gradientF_type;
		gradientF_type grF;

		typedef agg::span_gradient_alpha<agg::rgba8,
			interpolator_type,gradientF_type,alphaF_type> alpha_span_gen_type;
		alpha_span_gen_type alpha_span_gen(ip_alpha,grF,alphaF,0,150);

		typedef agg::span_image_filter_rgb_bilinear_clip<agg::pixfmt_rgba32,
			interpolator_type> pic_span_gen_type;
		pic_span_gen_type pic_span_gen(pixf_img,agg::rgba(1,1,1,1),ip_img);

		typedef agg::span_converter<pic_span_gen_type,
			alpha_span_gen_type> span_gen_type;
		span_gen_type span_gen(pic_span_gen,alpha_span_gen);

		agg::renderer_scanline_aa<renderer_base_type,
			span_allocator_type,span_gen_type> my_renderer(renb,span_alloc,span_gen);

		ras.move_to_d(leftUp.x(),leftUp.y()); 
		ras.line_to_d(leftUp.x(), rightDown.y()); 
		ras.line_to_d(rightDown.x(), rightDown.y()); 
		ras.line_to_d(rightDown.x(), leftUp.y()); 
		agg::render_scanlines(ras,sl,my_renderer);

		if(pdata)
		{
			delete[] pdata;
			pdata = NULL;
		}

		//用agg渲染透明面，使渲染的可以设置图片颜色
		agg::rasterizer_scanline_aa<> polygonRas; 
		agg::path_storage ps;
		agg::trans_affine mtx;
		typedef agg::conv_transform<agg::path_storage> ell_ct_type;
		ell_ct_type ctell(ps,mtx);
		ps.move_to(leftUp.x(),leftUp.y());
		ps.line_to(leftUp.x(), rightDown.y());
		ps.line_to(rightDown.x(), rightDown.y());
		ps.line_to(rightDown.x(), leftUp.y());
		polygonRas.add_path(ctell);
		osg::Vec4 polygonColor = GetColor(imageColor);

		if(imageColor != 0xFFFFFFFF)
			agg::render_scanlines_aa_solid(polygonRas,sl,renb,
			agg::rgba8(polygonColor.r() * 255,polygonColor.g() * 255,
			polygonColor.b() * 255,apha * 0.5));//图片透明度

		unsigned width = renb.width();
		unsigned height = renb.height();
		////0730测试  可行
		if(dataType == GL_RGBA)
		{//还原透明通道
			//return;
			unsigned char *data = image->data();
			int offset = height - leftUp.y();
			//chenpeng 0731
			int row = 0;
			for(int x = leftUp.y(); x < leftUp.y() + image->t();x++)
			{
				int col = 0;
				for(int y = leftUp.x(); y < leftUp.x() + image->s();y++)
				{
					unsigned char* a = data + ((image->t()-1-row) * image->s() + col ) * 4 + 3;
					buffer[((height-1-x) * width + y) * 4 + 3] = *a * imageOpacity / 100.0;
					col++;
				}
				row++;
			}
		}
	}
}

void CGlbGlobeMarkerLabelSymbol::DealText(osg::Vec2d leftUp,CGlbWString content,
	renb_type &renb,GlbRenderTextInfo *textInfo,CGlbFeature *feature)
{
	if(textInfo == NULL || content.size() == 0)
		return;

	glbInt32 textFontColor = 0x00000000;
	if(textInfo->color)
		textFontColor = textInfo->color->GetValue(feature);

	CGlbWString textFont = L"宋体";
	glbInt32 textSize = 1;
	if(textInfo->font)
		textFont = textInfo->font->GetValue(feature);//字体
	if(textInfo->size)
		textSize = textInfo->size->GetValue(feature);
	glbBool /*isBold = false,isItalic = false,*/isUnderline = false;
	//if(textInfo->isBold)
	//	isBold = textInfo->isBold->GetValue(feature);//粗体
	//if(textInfo->isItalic)
	//	isItalic = textInfo->isItalic->GetValue(feature);//斜体
	if(textInfo->isUnderline)
		isUnderline = textInfo->isUnderline->GetValue(feature);//下划线

	agg::rasterizer_scanline_aa<> ras;  
	agg::scanline_p8 sl; 

	osg::Vec4 textColor = GetColor(textFontColor);
	//if(isItalic)
	//	mpr_font->italic(true); //斜体
	//else 
	//	mpr_font->italic(false); //正常
	//if (isBold)
	//	mpr_font->weight(100000);//文字加粗
	//else
	//	mpr_font->weight(400);

	//根据图片与文字的轴模式设置起始位置
	double x = leftUp.x(),y = leftUp.y();
	double beginX = x,beginY = y;
	const wchar_t *text = content.c_str();
	bool glyphAvailable = false;

	agg::font_cache_manager<agg::font_engine_win32_tt_int16> *fontManager = 
		new agg::font_cache_manager<agg::font_engine_win32_tt_int16>(*mpr_font);

	for(;*text;text++) 
	{ 
		//取字模 
		const agg::glyph_cache* glyph = fontManager->glyph(*text);
		if(glyph) 
		{ 
			// 初始化gray8_adaptor实例 
			fontManager->init_embedded_adaptors(glyph, x, y); 
			agg::render_scanlines_aa_solid(fontManager->gray8_adaptor(),
				fontManager->gray8_scanline(), renb, agg::rgba8(textColor.r() * 255,
				textColor.g() * 255, textColor.b() * 255, textColor.a() * 255)); 

			if (*text == ' ')
			{
				beginX += glyph->advance_x;
				beginY += glyph->advance_y;
			}
			// 前进 
			x += glyph->advance_x; 
			y += glyph->advance_y; 
		} 
	}

	if(isUnderline)
	{
		agg::path_storage ps;
		ps.move_to(beginX,beginY + 2);//下划线总是遮挡字，把下划线往下移两个线宽
		ps.line_to(x,y + 2);
		agg::conv_stroke<agg::path_storage, agg::vcgen_markers_term> csps(ps); 
		csps.width(2);
		ras.add_path(csps);
		agg::render_scanlines_aa_solid(ras,sl,renb,
			agg::rgba8(textColor.r() * 255,textColor.g() * 255,textColor.b() * 255,255));
	}

	if(fontManager)
	{
		delete fontManager;
		fontManager = NULL;
	}
}

std::vector<CGlbWString> GlbGlobe::CGlbGlobeMarkerLabelSymbol::DealMultilineAlignText(
	GlbRenderTextInfo *textRenderInfo,CGlbFeature *feature,glbDouble &bytes)
{
	//计算字所需要的像素
	glbInt32 textSize = 0;
	CGlbWString content = L"";
	if(textRenderInfo->size)
		textSize = textRenderInfo->size->GetValue(feature);
	if(textRenderInfo->content)
		content = textRenderInfo->content->GetValue(feature);

	glbInt32 textCount = content.size();
	std::vector<int> cutContainer;
	for (int i = 0; i < textCount;++i)
	{
		wchar_t wchar = content.at(i);
		if (wchar == '\n' || wchar == '~')
			cutContainer.push_back(i + 1);
	}

	glbInt32 tempCount = cutContainer.size();
	if(tempCount > 0)
		cutContainer.push_back(textCount);

	std::vector<CGlbWString> wsContainer;
	int j = cutContainer.size()/* - 1*/;
	for (int i = 0; i < j;++i)
	{
		if (i == 0)
			wsContainer.push_back(content.substr(0,cutContainer.at(i) - 1));
		else if(i == j - 1)
			wsContainer.push_back(content.substr(cutContainer.at(i - 1),cutContainer.at(i) - cutContainer.at(i - 1)));
		else
			wsContainer.push_back(content.substr(cutContainer.at(i - 1),cutContainer.at(i) - cutContainer.at(i - 1) - 1));
	}

	////多行对齐，可以处理传进来的字符串，判断"\n"
	GlbGlobeMultilineAlignTypeEnum multilineAlign = GLB_MULTILINEALG_LEFT;
	if(textRenderInfo && textRenderInfo->multilineAlign)
		multilineAlign = (GlbGlobeMultilineAlignTypeEnum)textRenderInfo->multilineAlign->GetValue(feature);


	//glbDouble bytes = 0.0;//记录对齐方式参考字节数
	mpr_font->height(textSize);
	mpr_font->width(0);
	if(textRenderInfo && textRenderInfo->font)
	{
		CGlbWString textFont = textRenderInfo->font->GetValue(feature);//字体
		glbBool isBold = false,isItalic = false;
		if(textRenderInfo->isBold)
			isBold = textRenderInfo->isBold->GetValue(feature);//粗体
		if(textRenderInfo->isItalic)
			isItalic = textRenderInfo->isItalic->GetValue(feature);//斜体
		if(isItalic)
			mpr_font->italic(true); //斜体
		else
			mpr_font->italic(false);

		if (isBold)
			mpr_font->weight(100000);//文字加粗
		else
			mpr_font->weight(400);
		if(textFont != mpr_textFont)
		{
			mpr_font->create_font(textFont.ToString().c_str(),agg::glyph_ren_agg_gray8);
			mpr_textFont = textFont;
		}
	}

	agg::font_cache_manager<agg::font_engine_win32_tt_int16> *fontManager = 
		new agg::font_cache_manager<agg::font_engine_win32_tt_int16>(*mpr_font);

	for(glbInt32 i = 0; i < wsContainer.size();++i)
	{
		glbDouble x = 0.0,y = 0.0;
		CGlbWString tempContent = wsContainer.at(i);
		const wchar_t *text = tempContent.c_str();
		for(;*text;text++) 
		{
			//取字模 
			const agg::glyph_cache* glyph = fontManager->glyph(*text);
			if(glyph) 
			{
				// 初始化gray8_adaptor实例 
				fontManager->init_embedded_adaptors(glyph, x, y); 
				x += glyph->advance_x; 
				y += glyph->advance_y; 
			} 
		}
		if(x > bytes)
			bytes = x;
	}

	std::vector<CGlbWString> wsRenderContainer;
	if(multilineAlign == GLB_MULTILINEALG_RIGHT)
	{
		for (glbInt32 i = 0; i < wsContainer.size(); ++i)
		{
			glbDouble x = 0.0,y = 0.0;
			CGlbWString temp = wsContainer.at(i);
			const wchar_t *text = temp.c_str();
			for(;*text;text++) 
			{
				//取字模 
				const agg::glyph_cache* glyph = fontManager->glyph(*text);
				if(glyph) 
				{
					// 初始化gray8_adaptor实例 
					fontManager->init_embedded_adaptors(glyph, x, y); 
					x += glyph->advance_x; 
					y += glyph->advance_y; 
				} 
			}
			if(x != bytes)
			{
				glbInt32 tempSize = (bytes - x) / (textSize / 2);
				CGlbWString temp2 = L"";
				for(glbInt32 i = 0; i < tempSize;++i)
				{
					temp2.push_back(' ');
				}
				temp2 += temp;
				wsRenderContainer.push_back(temp2);
				continue;
			}
			wsRenderContainer.push_back(temp);
		}
	}
	else if(multilineAlign == GLB_MULTILINEALG_CENTER)
	{
		for (glbInt32 i = 0; i < wsContainer.size(); ++i)
		{
			CGlbWString temp = wsContainer.at(i);
			glbDouble x = 0.0,y = 0.0;
			const wchar_t *text = temp.c_str();
			for(;*text;text++) 
			{
				//取字模 
				const agg::glyph_cache* glyph = fontManager->glyph(*text);
				if(glyph) 
				{
					// 初始化gray8_adaptor实例 
					fontManager->init_embedded_adaptors(glyph, x, y); 
					x += glyph->advance_x; 
					y += glyph->advance_y; 
				} 
			}
			if(x != bytes)
			{
				glbInt32 tempSize = (bytes - x) / (textSize / 2) / 2;
				CGlbWString temp2 = L"";
				for(glbInt32 i = 0; i < tempSize;++i)
				{
					temp2.push_back(' ');
				}
				temp2 += temp;
				wsRenderContainer.push_back(temp2);
				continue;
			}
			wsRenderContainer.push_back(temp);
		}
	}
	else if(multilineAlign == GLB_MULTILINEALG_LEFT)
	{
		for (glbInt32 i = 0; i < wsContainer.size(); ++i)
		{
			CGlbWString temp = wsContainer.at(i);
			wsRenderContainer.push_back(temp);
		}
	}

	if(fontManager)
	{
		delete fontManager;
		fontManager = NULL;
	}

	return wsRenderContainer;
}

void GlbGlobe::CGlbGlobeMarkerLabelSymbol::DealComputeBufferSize( 
	glbInt32 multilineSize,glbInt32 textSize,CGlbWString textFont,
	CGlbWString content,glbInt32 bytes,
	GlbGlobeLabelRelationEnum labelRelation,
	GlbGlobeLabelAlignTypeEnum labelAlign,glbBool isItalic,
	osg::Image *tempImage,glbInt32 &sizeWidth,glbInt32 &sizeHeight,
	glbDouble &simplelineX,glbDouble &simplelineY,glbInt32 imageMaxSize)
{
	if(multilineSize == 0)//多行里没有文本，说明只有一行文本
	{
		mpr_font->create_font(textFont.ToString().c_str(),agg::glyph_ren_agg_gray8);
		mpr_font->height(textSize);
		mpr_font->width(0);
		if(textFont != mpr_textFont)
		{
			mpr_font->create_font(textFont.ToString().c_str(),agg::glyph_ren_agg_gray8);
			mpr_textFont = textFont;
		}
		const wchar_t *text = content.c_str();
		agg::font_cache_manager<agg::font_engine_win32_tt_int16> *fontManager = 
			new agg::font_cache_manager<agg::font_engine_win32_tt_int16>(*mpr_font);
		for(;*text;text++) 
		{
			//取字模 
			const agg::glyph_cache* glyph = fontManager->glyph(*text);
			if(glyph) 
			{
				// 初始化gray8_adaptor实例 
				fontManager->init_embedded_adaptors(glyph, simplelineX, simplelineY); 
				simplelineX += glyph->advance_x; 
				simplelineY += glyph->advance_y; 
			} 
		}

		if(fontManager)
		{
			delete fontManager;
			fontManager = NULL;
		}
	}

	if(labelRelation == GLB_LABEL_ON_IMAGE)
	{
		if (multilineSize > 0)
		{
			if(tempImage != NULL)
			{
				if (bytes > sizeWidth)
					sizeWidth = bytes + textSize;

				if (textSize * multilineSize > sizeHeight)
					sizeHeight = textSize * multilineSize;
				//else
				//	sizeHeight += textSize;
			}
			else
			{
				sizeWidth = bytes + textSize;
				sizeHeight = textSize * multilineSize + textSize;
			}
		}
		else
		{
			if(tempImage != NULL)
			{
				if (simplelineX > sizeWidth)
					sizeWidth = simplelineX + textSize;//最后一个字总是少一部分，所以多加一个字的宽度

				//0730 modefy
				//sizeHeight += textSize;
			}
			else
			{
				sizeWidth = simplelineX + textSize;
				sizeHeight = textSize + (glbDouble)textSize / 3;
			}
		}
	}
	else if (labelRelation == GLB_LABEL_SURROUND_IMAGE)
	{
		if (multilineSize > 0)
		{
			if(tempImage != NULL)
			{
				if (labelAlign == GLB_LABELALG_LEFTTOP ||
					labelAlign == GLB_LABELALG_LEFTBOTTOM ||
					labelAlign == GLB_LABELALG_RIGHTTOP ||
					labelAlign == GLB_LABELALG_RIGHTBOTTOM)
				{
					sizeWidth += bytes;
					if(labelAlign == GLB_LABELALG_LEFTTOP ||
						labelAlign == GLB_LABELALG_LEFTBOTTOM)
					{
						if(isItalic)
							sizeWidth += textSize / 5;
					}
					else
						sizeWidth += textSize;
					sizeHeight += textSize * multilineSize;
					//if(labelAlign == GLB_LABELALG_LEFTBOTTOM ||
					//	labelAlign == GLB_LABELALG_RIGHTBOTTOM)
					//	sizeHeight += textSize;
				}
				else if (labelAlign == GLB_LABELALG_LEFTCENTER ||
					labelAlign == GLB_LABELALG_RIGHTCENTER)
				{
					sizeWidth += bytes/* + textSize*/;
					if(labelAlign == GLB_LABELALG_LEFTCENTER && isItalic)
						sizeWidth += textSize / 5;
					
					if(labelAlign == GLB_LABELALG_RIGHTCENTER)
						sizeWidth += textSize;

					if (textSize * multilineSize > sizeHeight)
						sizeHeight = textSize * (multilineSize + 1);
				}
				else if (labelAlign == GLB_LABELALG_CENTERTOP ||
					labelAlign == GLB_LABELALG_CENTERBOTTOM)
				{
					if (bytes > sizeWidth)
						sizeWidth = bytes + textSize;
					sizeHeight += textSize * multilineSize;
					sizeHeight += 5;
				}
				else if(labelAlign == GLB_LABELALG_CENTERCENTER)//按照 LEFTTOP写的
				{
					sizeWidth += bytes;
					sizeHeight += textSize * multilineSize;
				}
			}
			else
			{
				sizeWidth = bytes + textSize;
				sizeHeight = textSize * multilineSize + textSize;
			}
		}
		else
		{
			if(tempImage != NULL && imageMaxSize != 0)
			{
				if(labelAlign == GLB_LABELALG_LEFTTOP)
				{
					sizeWidth += simplelineX;
					sizeHeight += textSize;
				}
				else if(labelAlign == GLB_LABELALG_LEFTBOTTOM ||
					labelAlign == GLB_LABELALG_RIGHTTOP ||
					labelAlign == GLB_LABELALG_RIGHTBOTTOM)
				{
					sizeWidth += simplelineX;
					if(labelAlign == GLB_LABELALG_RIGHTTOP ||
						labelAlign == GLB_LABELALG_RIGHTBOTTOM)
						sizeWidth += textSize;
					if(labelAlign == GLB_LABELALG_RIGHTTOP)
						sizeHeight += textSize;
					else
						sizeHeight += textSize * 2;
				}
				else if (labelAlign == GLB_LABELALG_LEFTCENTER||
					labelAlign == GLB_LABELALG_RIGHTCENTER)
				{
					sizeWidth += simplelineX/* + textSize / 5*/;
				}
				else if (labelAlign == GLB_LABELALG_CENTERTOP ||
					labelAlign == GLB_LABELALG_CENTERBOTTOM)
				{
					if (simplelineX > sizeWidth)
						sizeWidth = simplelineX + textSize;
					sizeHeight += textSize/* * 2*/;
				}
				else if(labelAlign == GLB_LABELALG_CENTERCENTER)//特例
				{
					//CENTERCENTER 完全是按照 LEFTTOP写的
					sizeWidth += simplelineX;
					sizeHeight += textSize;
				}
			}
			else
			{
				sizeWidth = simplelineX + textSize / 5;//加粗和倾斜的时候最后一个字会部分缺失
				sizeHeight = textSize + textSize / 5/* * 2*/;//chenpeng0822
			}
		}
	}
}

void GlbGlobe::CGlbGlobeMarkerLabelSymbol::DealRenderImageAndText(
	glbBool isEmpty,std::vector<CGlbWString> wsRenderContainer,
	CGlbWString content,glbDouble &simplelineX,glbInt32 bytes,glbInt32 textSize,
	osg::Image *tempImage,GlbRenderImageInfo *imageInfo,GlbRenderTextInfo *textInfo,
	GlbGlobeLabelRelationEnum labelRelation,GlbGlobeLabelAlignTypeEnum labelAlign,
	glbInt32 sizeWidth,glbInt32 sizeHeight,renb_type &renb,unsigned char * buffer,CGlbFeature *feature)
{
	if(isEmpty == false)
	{
		glbFloat x = 0.0, y = 0.0;
		glbBool isItalic = false;
		if(textInfo && textInfo->isItalic && textInfo->isItalic->GetValue(feature))
			isItalic = true;
		if (wsRenderContainer.size() > 0)
		{
			if(tempImage != NULL)
			{
				if (labelRelation == GLB_LABEL_ON_IMAGE)
				{
					if (labelAlign == GLB_LABELALG_LEFTTOP)
					{
						DealImage(osg::Vec2(0,0),osg::Vec2(tempImage->s(),tempImage->t()),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						for (int i = 0; i < (int)wsRenderContainer.size();++i)
							DealText(osg::Vec2(0 , (i + 1)* textSize),wsRenderContainer.at(i),renb,textInfo,feature);
					}
					else if (labelAlign == GLB_LABELALG_LEFTBOTTOM)
					{
						DealImage(osg::Vec2(0,0),osg::Vec2(tempImage->s(),tempImage->t()),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						for (int i = 0; i < (int)wsRenderContainer.size();++i)
							DealText(osg::Vec2(0 , sizeHeight - ((int)wsRenderContainer.size() - i)* textSize + textSize * 4 / 5),wsRenderContainer.at(i),renb,textInfo,feature);
					}
					else if (labelAlign == GLB_LABELALG_LEFTCENTER)
					{
						DealImage(osg::Vec2(0,0),osg::Vec2(tempImage->s(),tempImage->t()),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						for (int i = 0; i < wsRenderContainer.size();++i)
							DealText(osg::Vec2(0 , sizeHeight / 2 - (int)wsRenderContainer.size() * textSize / 2 + (i + 1) * textSize),wsRenderContainer.at(i),renb,textInfo,feature);
					}
					else if (labelAlign == GLB_LABELALG_CENTERTOP)
					{
						if(sizeWidth > tempImage->s())
							DealImage(osg::Vec2(sizeWidth / 2 - tempImage->s() / 2,0),osg::Vec2(sizeWidth / 2 - tempImage->s() / 2 + tempImage->s(),tempImage->t()),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						else
							DealImage(osg::Vec2(0,0),osg::Vec2(tempImage->s(),tempImage->t()),labelRelation,tempImage,renb,buffer,imageInfo,feature);

						for (int i = 0; i < wsRenderContainer.size();++i)
							DealText(osg::Vec2(sizeWidth / 2 - (bytes/* + textSize*/) / 2 , (i + 1)* textSize),wsRenderContainer.at(i),renb,textInfo,feature);
					}
					else if (labelAlign == GLB_LABELALG_CENTERCENTER)
					{
						if(sizeWidth > tempImage->s())
							DealImage(osg::Vec2(sizeWidth / 2 - tempImage->s() / 2,0),osg::Vec2(sizeWidth / 2 - tempImage->s() / 2 + tempImage->s(),tempImage->t()),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						else
							DealImage(osg::Vec2(0,0),osg::Vec2(tempImage->s(),tempImage->t()),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						for (int i = 0; i < wsRenderContainer.size();++i)
							DealText(osg::Vec2(sizeWidth / 2 - (bytes/* + textSize*/) / 2 , sizeHeight / 2 - (int)wsRenderContainer.size() * textSize / 2 + (i + 1) * textSize),wsRenderContainer.at(i),renb,textInfo,feature);
					}
					else if (labelAlign == GLB_LABELALG_CENTERBOTTOM)
					{
						if(sizeWidth > tempImage->s())
							DealImage(osg::Vec2(sizeWidth / 2 - tempImage->s() / 2,0),osg::Vec2(sizeWidth / 2 - tempImage->s() / 2 + tempImage->s(),tempImage->t()),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						else
							DealImage(osg::Vec2(0,0),osg::Vec2(tempImage->s(),tempImage->t()),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						for (int i = 0; i < wsRenderContainer.size();++i)
							DealText(osg::Vec2(sizeWidth / 2 - (bytes/* + textSize*/) / 2 , sizeHeight - ((int)wsRenderContainer.size() - i)* textSize + textSize * 4 / 5),wsRenderContainer.at(i),renb,textInfo,feature);
					}
					else if (labelAlign == GLB_LABELALG_RIGHTTOP)
					{
						if(sizeWidth > tempImage->s())
							DealImage(osg::Vec2(sizeWidth - tempImage->s(),0),osg::Vec2(sizeWidth,tempImage->t()),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						else
							DealImage(osg::Vec2(0,0),osg::Vec2(tempImage->s(),tempImage->t()),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						for (int i = 0; i < wsRenderContainer.size();++i)
							DealText(osg::Vec2(sizeWidth - (bytes/* + textSize*/) , (i + 1)* textSize),wsRenderContainer.at(i),renb,textInfo,feature);
					}
					else if (labelAlign == GLB_LABELALG_RIGHTCENTER)
					{
						if(sizeWidth > tempImage->s())
							DealImage(osg::Vec2(sizeWidth - tempImage->s(),0),osg::Vec2(sizeWidth,tempImage->t()),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						else
							DealImage(osg::Vec2(0,0),osg::Vec2(tempImage->s(),tempImage->t()),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						for (int i = 0; i < wsRenderContainer.size();++i)
							DealText(osg::Vec2(sizeWidth - (bytes/* + textSize*/) ,  sizeHeight / 2 - (int)wsRenderContainer.size() * textSize / 2 + (i + 1) * textSize),wsRenderContainer.at(i),renb,textInfo,feature);
					}
					else if (labelAlign == GLB_LABELALG_RIGHTBOTTOM)
					{
						if(sizeWidth > tempImage->s())
							DealImage(osg::Vec2(sizeWidth - tempImage->s(),0),osg::Vec2(sizeWidth,tempImage->t()),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						else
							DealImage(osg::Vec2(0,0),osg::Vec2(tempImage->s(),tempImage->t()),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						for (int i = 0; i < wsRenderContainer.size(); ++i)
							DealText(osg::Vec2(sizeWidth - (bytes/* + textSize*/) ,   sizeHeight - ((int)wsRenderContainer.size() - i)* textSize + textSize * 4 / 5),wsRenderContainer.at(i),renb,textInfo,feature);
					}
				}
				else if (labelRelation == GLB_LABEL_SURROUND_IMAGE)
				{
					if (labelAlign == GLB_LABELALG_LEFTTOP)
					{
						x = bytes;
						if(isItalic)
							x += textSize / 5;
						DealImage(osg::Vec2(x,(int)wsRenderContainer.size() * textSize),osg::Vec2(sizeWidth,sizeHeight),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						for (int i = 0; i < (int)wsRenderContainer.size();++i)
							DealText(osg::Vec2(0 , (i + 1)* textSize),wsRenderContainer.at(i),renb,textInfo,feature);
					}
					else if (labelAlign == GLB_LABELALG_LEFTBOTTOM)
					{
						x = bytes;
						if(isItalic)
							x += textSize / 5;
						DealImage(osg::Vec2(x,0),osg::Vec2(sizeWidth,sizeHeight - (int)wsRenderContainer.size() * textSize - textSize),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						for (int i = 0; i < (int)wsRenderContainer.size();++i)
							DealText(osg::Vec2(0 , sizeHeight - i * textSize - textSize),wsRenderContainer.at(wsRenderContainer.size() - i - 1),renb,textInfo,feature);
					}
					else if (labelAlign == GLB_LABELALG_LEFTCENTER)
					{
						//必须偏移一个字节，因为先渲染的文字后渲染的图片，不偏移文字加粗倾斜的时候会被图片部分遮挡
						x = bytes;
						if(isItalic)
							x += textSize / 5;
						DealImage(osg::Vec2(x,0),osg::Vec2(sizeWidth,tempImage->t()/*sizeHeight*/),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						for (int i = 0; i < (int)wsRenderContainer.size();++i)
							DealText(osg::Vec2(0 , sizeHeight / 2 - (int)wsRenderContainer.size() * textSize / 2 + (i + 1) * textSize),wsRenderContainer.at(i),renb,textInfo,feature);
					}
					else if (labelAlign == GLB_LABELALG_CENTERTOP)
					{
						if(sizeWidth > tempImage->s())
							DealImage(osg::Vec2(sizeWidth / 2 - tempImage->s() / 2,(int)wsRenderContainer.size() * textSize + textSize / 5),
							osg::Vec2(sizeWidth / 2 - tempImage->s() / 2 + tempImage->s(),(int)wsRenderContainer.size() * textSize + tempImage->t() + textSize / 5),
							labelRelation,tempImage,renb,buffer,imageInfo,feature);
						else
							DealImage(osg::Vec2(0,(int)wsRenderContainer.size() * textSize + textSize / 5),osg::Vec2(tempImage->s(),(int)wsRenderContainer.size() * textSize + tempImage->t() + 5),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						for (int i = 0; i < wsRenderContainer.size();++i)
							DealText(osg::Vec2(sizeWidth / 2 - (bytes/* + textSize*/) / 2 , (i + 1)* textSize),wsRenderContainer.at(i),renb,textInfo,feature);
					}
					else if (labelAlign == GLB_LABELALG_CENTERCENTER)
					{
						DealImage(osg::Vec2(bytes,(int)wsRenderContainer.size() * textSize),osg::Vec2(sizeWidth,sizeHeight),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						for (int i = 0; i < (int)wsRenderContainer.size();++i)
							DealText(osg::Vec2(0 , (i + 1)* textSize),wsRenderContainer.at(i),renb,textInfo,feature);
					}
					else if (labelAlign == GLB_LABELALG_CENTERBOTTOM)
					{
						if(sizeWidth > tempImage->s())
							DealImage(osg::Vec2(sizeWidth / 2 - tempImage->s() / 2,0),osg::Vec2(sizeWidth / 2 - tempImage->s() / 2 + tempImage->s(),tempImage->t()),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						else
							DealImage(osg::Vec2(0,0),osg::Vec2(tempImage->s(),tempImage->t()),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						for (int i = 0; i < wsRenderContainer.size();++i)
							DealText(osg::Vec2(sizeWidth / 2 - (bytes/* + textSize*/) / 2 , sizeHeight - ((int)wsRenderContainer.size() - i - 1)* textSize - 5),wsRenderContainer.at(i),renb,textInfo,feature);
					}
					else if (labelAlign == GLB_LABELALG_RIGHTTOP)
					{
						DealImage(osg::Vec2(0,wsRenderContainer.size() * textSize),osg::Vec2(sizeWidth - (bytes + textSize),sizeHeight),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						for (int i = 0; i < wsRenderContainer.size();++i)
							DealText(osg::Vec2(sizeWidth - (bytes + textSize) , (i + 1)* textSize),wsRenderContainer.at(i),renb,textInfo,feature);
					}
					else if (labelAlign == GLB_LABELALG_RIGHTCENTER)
					{
						DealImage(osg::Vec2(0,0),osg::Vec2(sizeWidth - (bytes + textSize),tempImage->t()),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						for (int i = 0; i < wsRenderContainer.size();++i)
							DealText(osg::Vec2(sizeWidth - (bytes + textSize) ,  sizeHeight / 2 - (int)wsRenderContainer.size() * textSize / 2 + (i + 1) * textSize),wsRenderContainer.at(i),renb,textInfo,feature);
					}
					else if (labelAlign == GLB_LABELALG_RIGHTBOTTOM)
					{
						DealImage(osg::Vec2(0,0),osg::Vec2(sizeWidth - (bytes + textSize),sizeHeight - wsRenderContainer.size() * textSize - textSize),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						for (int i = 0; i < wsRenderContainer.size(); ++i)
							DealText(osg::Vec2(sizeWidth - (bytes + textSize) ,   sizeHeight - ((int)wsRenderContainer.size() - i )* textSize/* - textSize*/),wsRenderContainer.at(i),renb,textInfo,feature);
					}
				}
			}
			else
			{
				for (int i = 0; i < (int)wsRenderContainer.size();++i)
					DealText(osg::Vec2(0 , sizeHeight - i * textSize - textSize),wsRenderContainer.at((int)wsRenderContainer.size() - i - 1),renb,textInfo,feature);
			}
		}
		else
		{
			if(tempImage != NULL)
			{
				if(labelRelation == GLB_LABEL_ON_IMAGE)
				{
					if (labelAlign == GLB_LABELALG_CENTERTOP ||
						labelAlign == GLB_LABELALG_CENTERCENTER ||
						labelAlign == GLB_LABELALG_CENTERBOTTOM)
					{
						if(sizeWidth > tempImage->s())
							DealImage(osg::Vec2(sizeWidth/2 - tempImage->s() / 2,0),osg::Vec2(sizeWidth/2 - tempImage->s() / 2 + tempImage->s(),tempImage->t()),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						else
							DealImage(osg::Vec2(0,0),osg::Vec2(tempImage->s(),tempImage->t()),labelRelation,tempImage,renb,buffer,imageInfo,feature);
					}
					else if (labelAlign == GLB_LABELALG_RIGHTTOP ||
						labelAlign == GLB_LABELALG_RIGHTCENTER ||
						labelAlign == GLB_LABELALG_RIGHTBOTTOM)
					{
						if (sizeWidth > tempImage->s())
							DealImage(osg::Vec2(sizeWidth - tempImage->s(),0),osg::Vec2(sizeWidth,tempImage->t()),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						else
							DealImage(osg::Vec2(0,0),osg::Vec2(tempImage->s(),tempImage->t()),labelRelation,tempImage,renb,buffer,imageInfo,feature);
					}
					else
						DealImage(osg::Vec2(0,0),osg::Vec2(tempImage->s(),tempImage->t()),labelRelation,tempImage,renb,buffer,imageInfo,feature);

					double x2 = 0.0,y2 = 0.0;
					if (labelAlign == GLB_LABELALG_LEFTTOP)
						x2 = 0.0, y2 = textSize;
					else if (labelAlign == GLB_LABELALG_LEFTCENTER)
						x2 = 0.0, y2 = sizeHeight / 2;
					else if (labelAlign == GLB_LABELALG_LEFTBOTTOM)
						x2 = 0.0, y2 = sizeHeight - textSize / 5;
					else if (labelAlign == GLB_LABELALG_CENTERTOP)
						x2 = sizeWidth / 2 - (simplelineX/* + textSize*/) / 2, y2 = textSize;
					else if (labelAlign == GLB_LABELALG_CENTERCENTER)
						x2 = sizeWidth / 2 - (simplelineX/* + textSize*/) / 2, y2 = sizeHeight / 2/* - textSize / 2*/;
					else if(labelAlign == GLB_LABELALG_CENTERBOTTOM)
						x2 = sizeWidth / 2 - (simplelineX /*+ textSize*/) / 2, y2 = sizeHeight - textSize / 5;
					else if(labelAlign == GLB_LABELALG_RIGHTTOP)
						x2 = sizeWidth - (simplelineX/* + textSize*/),y2 = textSize;
					else if (labelAlign == GLB_LABELALG_RIGHTCENTER)
						x2 = sizeWidth - (simplelineX/* + textSize*/),y2 = sizeHeight / 2;
					else if(labelAlign == GLB_LABELALG_RIGHTBOTTOM)
						x2 = sizeWidth - (simplelineX/* + textSize*/),y2 = sizeHeight - textSize / 5;
					DealText(osg::Vec2(x2,y2),content,renb,textInfo,feature);
				}
				else if (labelRelation == GLB_LABEL_SURROUND_IMAGE)
				{
					if(tempImage)
					{
						if (labelAlign == GLB_LABELALG_LEFTTOP)
							DealImage(osg::Vec2(simplelineX,textSize),
							osg::Vec2(sizeWidth,sizeHeight),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						else if (labelAlign == GLB_LABELALG_LEFTCENTER)
							DealImage(osg::Vec2(simplelineX/* + textSize*/,0),
							osg::Vec2(sizeWidth,sizeHeight),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						else if (labelAlign == GLB_LABELALG_LEFTBOTTOM)
							DealImage(osg::Vec2(simplelineX,0),
							osg::Vec2(sizeWidth,sizeHeight - textSize * 2),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						else if (labelAlign == GLB_LABELALG_CENTERTOP)
						{
							//160422添加textSize / 5是为了解决png图片添加中上字体的时候在图片和文字之间出现一条白线
							if(sizeWidth > tempImage->s())
								DealImage(osg::Vec2(sizeWidth / 2 - tempImage->s() / 2,textSize/* * 2*/ + textSize / 5),
								osg::Vec2(sizeWidth / 2 - tempImage->s() / 2 + tempImage->s(),sizeHeight),labelRelation,tempImage,renb,buffer,imageInfo,feature);
							else
								DealImage(osg::Vec2(0,textSize/* * 2*/),
								osg::Vec2(tempImage->s(),sizeHeight),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						}
						else if (labelAlign == GLB_LABELALG_CENTERCENTER)
							DealImage(osg::Vec2(simplelineX,textSize),
							osg::Vec2(sizeWidth,sizeHeight),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						else if(labelAlign == GLB_LABELALG_CENTERBOTTOM)
						{
							if(sizeWidth > tempImage->s())
								DealImage(osg::Vec2(osg::Vec2(sizeWidth / 2 - tempImage->s() / 2,0)),
								osg::Vec2(sizeWidth / 2 - tempImage->s() / 2 + tempImage->s(),tempImage->t()),labelRelation,tempImage,renb,buffer,imageInfo,feature);
							else
								DealImage(osg::Vec2(0,0),
								osg::Vec2(tempImage->s(),tempImage->t()),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						}
						else if(labelAlign == GLB_LABELALG_RIGHTTOP)
						{
							DealImage(osg::Vec2(0,textSize),
								osg::Vec2(sizeWidth - (simplelineX + textSize),sizeHeight),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						}
						else if (labelAlign == GLB_LABELALG_RIGHTCENTER)
							DealImage(osg::Vec2(0,0),
							osg::Vec2(sizeWidth - (simplelineX/* + textSize*/),sizeHeight),labelRelation,tempImage,renb,buffer,imageInfo,feature);
						else if(labelAlign == GLB_LABELALG_RIGHTBOTTOM)
							DealImage(osg::Vec2(0,0),osg::Vec2(tempImage->s(),tempImage->t()),
							labelRelation,tempImage,renb,buffer,imageInfo,feature);
					}

					double x2 = 0.0,y2 = 0.0;
					if (labelAlign == GLB_LABELALG_LEFTTOP)
						x2 = 0.0, y2 = textSize;
					else if (labelAlign == GLB_LABELALG_LEFTCENTER)
						x2 = 0.0,y2 = sizeHeight / 2;
					else if (labelAlign == GLB_LABELALG_LEFTBOTTOM)
						x2 = 0.0,y2 = sizeHeight - textSize;
					else if (labelAlign == GLB_LABELALG_CENTERTOP)
						x2 = sizeWidth / 2 - (simplelineX/* + textSize*/) / 2,y2 = textSize;
					else if (labelAlign == GLB_LABELALG_CENTERCENTER)
						x2 = 0.0, y2 = textSize;
					else if(labelAlign == GLB_LABELALG_CENTERBOTTOM)
						x2 = sizeWidth / 2 - (simplelineX/* + textSize*/) / 2,y2 = sizeHeight/* - textSize*/;
					else if(labelAlign == GLB_LABELALG_RIGHTTOP)
						x2 = sizeWidth - (simplelineX + textSize),y2 = textSize;
					else if (labelAlign == GLB_LABELALG_RIGHTCENTER)
						x2 = sizeWidth- (simplelineX/* + textSize*/),y2 = sizeHeight / 2;
					else if(labelAlign == GLB_LABELALG_RIGHTBOTTOM)
						x2 = sizeWidth - (simplelineX + textSize),y2 = sizeHeight - textSize;
					DealText(osg::Vec2(x2,y2),content,renb,textInfo,feature);
				}
			}
			else
			{
				DealText(osg::Vec2(0.0,sizeHeight - textSize / 5),content,renb,textInfo,feature);
			}
		}
	}
	else
	{
		if(tempImage != NULL)
		{
			DealImage(osg::Vec2(0,0),osg::Vec2(tempImage->s(),tempImage->t()),
				labelRelation,tempImage,renb,buffer,imageInfo,feature);
		}
	}
}

osg::Geometry * GlbGlobe::CGlbGlobeMarkerLabelSymbol::CreateOutlineQuad( osg::Vec3 cornerVec,osg::Vec3 widthVec,osg::Vec3 heightVec )
{
	osg::Geometry *geom = new osg::Geometry;
	osg::ref_ptr<osg::StateSet> geomState = geom->getOrCreateStateSet();
	osg::ref_ptr<osg::Material> geomMaterial = 
		dynamic_cast<osg::Material *>(geomState->getAttribute(osg::StateAttribute::MATERIAL));
	if(geomMaterial == NULL)
		geomMaterial = new osg::Material;
	static int i = 0;
	geomMaterial->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(1,1,0,1));
	geomMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,0));
	geomState->setAttribute(geomMaterial,osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
	osg::ref_ptr<osg::Vec3dArray> vertexArray = new osg::Vec3dArray(4);
	(*vertexArray)[0] = cornerVec + heightVec;
	(*vertexArray)[1] = cornerVec;
	(*vertexArray)[2] = cornerVec + widthVec;
	(*vertexArray)[3] = cornerVec + widthVec + heightVec;
	geom->setVertexArray(vertexArray);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);
	osg::ref_ptr<osg::Vec3dArray> normals = new osg::Vec3dArray(1);
	(*normals)[0] = widthVec ^ heightVec;
	(*normals)[0].normalize();
	geom->setNormalArray(normals);
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	geom->addPrimitiveSet(new DrawArrays(PrimitiveSet::LINE_LOOP,0,4));
	return geom;
}

void GlbGlobe::CGlbGlobeMarkerLabelSymbol::DrawToImage( CGlbGlobeRObject *obj,IGlbGeometry* geom, glbByte *image/*RGBA*/,
	glbInt32 imageW, glbInt32 imageH,CGlbExtent &ext,glbBool isDirectDraw )
{
	agg::rendering_buffer rbuf(image, imageW, imageH, -imageW * 4);
	agg::pixfmt_rgba32 pixf(rbuf);
	mpr_isDirectDraw = isDirectDraw;
	DealAggRender(rbuf,obj,geom,imageW,imageH,ext);	
}

IGlbGeometry * GlbGlobe::CGlbGlobeMarkerLabelSymbol::GetOutline( CGlbGlobeRObject *obj,IGlbGeometry *geo )
{
	if(obj->GetAltitudeMode() == GLB_ALTITUDEMODE_ONTERRAIN)
	{
		GlbRenderInfo *renderInfo = obj->GetRenderInfo();
		if(renderInfo == NULL)
			return NULL;
		GlbMarkerLabelSymbolInfo *markerLabel = 
			static_cast<GlbMarkerLabelSymbolInfo *>(renderInfo);
		if(markerLabel == NULL)
			return NULL;
		osg::ref_ptr<osg::Vec2dArray> geoPoints = new osg::Vec2dArray();
		GlbGeometryTypeEnum geoType = geo->GetType();
		glbDouble ptx,pty;
		switch(geoType)
		{
		case GLB_GEO_POINT:
			{
				CGlbPoint *pt = dynamic_cast<CGlbPoint*>(geo);
				pt->GetXY(&ptx,&pty);
				geoPoints->push_back(osg::Vec2d(ptx,pty));
			}
			break;
		case GLB_GEO_MULTIPOINT:
			{
				CGlbMultiPoint* pts = dynamic_cast<CGlbMultiPoint*>(geo);
				glbInt32 ptCnt = pts->GetCount();
				for (glbInt32 i = 0; i < ptCnt; i++)
				{
					pts->GetPoint(i,&ptx,&pty);
					geoPoints->push_back(osg::Vec2d(ptx,pty));
				}				
			}
			break;
		}

		int ptCnt = geoPoints->size();
		if (ptCnt==1)
		{
			mpt_outline = DealOutline(geoPoints->at(0),markerLabel,obj);
		}
		else if (ptCnt>1)
		{
			CGlbMultiLine* multiln = NULL;
			for (glbInt32 k = 0; k < ptCnt; k++)
			{
				glbref_ptr<IGlbGeometry> geom = DealOutline(geoPoints->at(k),markerLabel,obj);
				if (geom)
				{
					geoType = geom->GetType();
					if (geoType==GLB_GEO_LINE)
					{
						CGlbLine* ln = dynamic_cast<CGlbLine*>(geom.get());
						if (ln)
						{
							if (!multiln) multiln = new CGlbMultiLine();
							multiln->AddLine(ln);
						}
					}					
				}
			}
			mpt_outline = multiln;
		}
	}
	return mpt_outline.release();
}

IGlbGeometry * GlbGlobe::CGlbGlobeMarkerLabelSymbol::DealOutline( osg::Vec2d centerPosition, GlbMarkerLabelSymbolInfo *markerInfo,CGlbGlobeRObject *obj )
{
	CGlbLine *line = new CGlbLine;
	double rotation = 0.0;
	glbref_ptr<CGlbFeature> feature = obj->GetFeature();
	if(markerInfo->yaw)
		rotation = markerInfo->yaw->GetValue(feature.get());
	rotation = agg::deg2rad(rotation);
	osg::Vec3d centerPos(centerPosition.x(),centerPosition.y(),0);
	osg::Matrixd localToWorld;
	if(GLB_GLOBETYPE_GLOBE == obj->GetGlobe()->GetType())
	{
		g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(
			osg::DegreesToRadians(centerPos.y()),osg::DegreesToRadians(centerPos.x()),centerPos.z(),localToWorld);
		g_ellipsoidModel->convertLatLongHeightToXYZ(
			osg::DegreesToRadians(centerPos.y()),osg::DegreesToRadians(centerPos.x()),centerPos.z(),
			centerPos.x(),centerPos.y(),centerPos.z());
	}
	glbDouble halfWidth = 0.0;
	glbDouble halfHeight = 0.0;
	mpr_isComputeOutline = true;
	Draw(obj,NULL);
	mpr_isComputeOutline = false;
	halfWidth = mpr_halfWidth;
	halfHeight = mpr_halfHeight;
	osg::Vec2d pts[4];
	pts[0].set(-halfWidth, halfHeight);
	pts[1].set(-halfWidth, -halfHeight);
	pts[2].set(halfWidth, -halfHeight);
	pts[3].set(halfWidth, halfHeight);

	for (int k = 0; k <4; k++)
	{			
		double theta = atan2(pts[k].y(),pts[k].x());
		double len = pts[k].length();//sqrt(pts[k].x()*pts[k].x()+pts[k].y()*pts[k].y());
		double xx = len * cos(theta+rotation);
		double yy = len * sin(theta+rotation);

		osg::Vec3d pt(xx,yy,0);
		if (GLB_GLOBETYPE_GLOBE == obj->GetGlobe()->GetType())
		{
			pt = localToWorld.preMult(pt);  // pt * localToWorld
			g_ellipsoidModel->convertXYZToLatLongHeight(pt.x(),pt.y(),pt.z(),pt.y(),pt.x(),pt.z());
			line->AddPoint(osg::RadiansToDegrees(pt.x()),osg::RadiansToDegrees(pt.y()));
		}
		else
			line->AddPoint(centerPosition.x()+pt.x(),centerPosition.y()+pt.y());	
	}
	return line;
}

void GlbGlobe::CGlbGlobeMarkerLabelSymbol::DealAggRender( agg::rendering_buffer rbuf,CGlbGlobeRObject *obj,
	IGlbGeometry *geom, glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext )
{
	agg::path_storage ps;
	agg::trans_affine mtx;

	tempExtent = ext;
	tempImageW = imageW;
	tempImageH = imageH;

	GlbRenderInfo *renderInfo = obj->GetRenderInfo();
	if(renderInfo == NULL)
		return;

	GlbMarkerLabelSymbolInfo *markerLabel = 
		static_cast<GlbMarkerLabelSymbolInfo *>(renderInfo);

	if (!markerLabel)
		return;

	CGlbFeature *feature = obj->GetFeature();

	glbref_ptr<IGlbGeometry> outline = NULL;

	CGlbMultiLine *multiline = dynamic_cast<CGlbMultiLine *>(geom);
	glbref_ptr<CGlbLine> line = dynamic_cast<CGlbLine *>(geom);

	// 矢量绘制到renb
	if(line)
	{
		DealAggWrapTexture(rbuf,obj,markerLabel,feature,line.get());
	}
	else if(multiline)
	{
		for (glbInt32 i = 0; i < multiline->GetCount(); i++)
		{
			line = const_cast<CGlbLine *>(multiline->GetLine(i));
			DealAggWrapTexture(rbuf,obj,markerLabel,feature,line.get());
		}
	}
}

osg::ref_ptr<osg::Vec2dArray> GlbGlobe::CGlbGlobeMarkerLabelSymbol::GetPointFromGeom( IGlbGeometry *geom )
{
	osg::ref_ptr<osg::Vec2dArray> pts = new osg::Vec2dArray;
	glbDouble ptx,pty;
	GlbGeometryTypeEnum geomType = geom->GetType();
	switch(geomType)	
	{
	case GLB_GEO_POINT:
	case GLB_GEO_MULTIPOINT:
		{
			CGlbMultiPoint* multPt = dynamic_cast<CGlbMultiPoint*>(geom);
			glbInt32 ptCnt = multPt->GetCount();
			for (glbInt32 k = 0; k < ptCnt; k++)
			{
				multPt->GetPoint(k,&ptx,&pty);
				pts->push_back(osg::Vec2d(ptx,pty));
			}
		}
		break;
	}
	return pts;
}

void GlbGlobe::CGlbGlobeMarkerLabelSymbol::FillDrawVertexArray( agg::path_storage &ps,IGlbGeometry *outline )
{
	CGlbLine *line = dynamic_cast<CGlbLine *>(outline);
	if(line == NULL)
		return;

	const glbDouble *points = line->GetPoints();
	glbInt32 count = line->GetCount();
	for (glbInt32 i = 0; i < count; ++i)
	{
		//不做矩阵变换，转换坐标用这个
		osg::Vec2d point(points[2 * i],points[2 * i + 1]);

		double new_x = (point.x() - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
		double new_y = (point.y() - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));
		if(i == 0)
		{
			ps.move_to(new_x,new_y);
			continue;
		}
		ps.line_to(new_x,new_y);
	}
}

void GlbGlobe::CGlbGlobeMarkerLabelSymbol::DealAggWrapTexture( agg::rendering_buffer rbuf,
	CGlbGlobeRObject *obj,GlbMarkerLabelSymbolInfo *markerInfo, CGlbFeature *feature,IGlbGeometry *outline )
{
	if(obj->GetCustomData() == NULL || mpr_isDirectDraw == true)
	{
		Draw(obj,NULL);
	}

	osg::Image *image = dynamic_cast<osg::Image *>(obj->GetCustomData());
	if(image == NULL)
		return;	

	cglbExtent = const_cast<CGlbExtent *>(outline->GetExtent());
	typedef agg::renderer_base<agg::pixfmt_rgba32> renderer_base_type;

	agg::rendering_buffer rbuf_img(NULL,0,0,0);
	GLenum dataType = (GLenum)(image->getPixelFormat());

	unsigned char* pdata=NULL;
	if(dataType == GL_RGB)
		rbuf_img = agg::rendering_buffer(image->data(),image->s(),image->t(), 3 * image->s());
	else if(dataType == GL_RGBA)
	{
		unsigned char *data = image->data();
		rbuf_img = agg::rendering_buffer(data,image->s(),image->t(),  4 * image->s());
	}

	agg::path_storage canvas;
	CGlbLine *line = dynamic_cast<CGlbLine *>(outline);

	if(line == NULL)
		return;

	const glbDouble *points = line->GetPoints();
	glbInt32 count = line->GetCount();
	for (glbInt32 i = 0; i <count; i++)
	{
		//不做矩阵变换，转换坐标用这个
		osg::Vec2d point(points[2 * i],points[2 * i + 1]);		

		double new_x = (point.x() - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
		double new_y = (point.y() - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));

		if(i == 0)
		{
			canvas.move_to(new_x,new_y);
			continue;
		}
		canvas.line_to(new_x,new_y);		
	}
	canvas.close_polygon();

	agg::trans_affine imageMatrix;
	imageMatrix.multiply(agg::trans_affine_translation(-image->s()*0.5,-image->t()*0.5));
	imageMatrix.multiply(agg::trans_affine_rotation(0));
	imageMatrix.multiply(agg::trans_affine_translation(image->s()*0.5,image->t()*0.5));

	double imgscalex = double(tempImageW) / image->s() * (cglbExtent->GetXWidth() / tempExtent.GetXWidth());
	double imgscaley = double(tempImageH) / image->t() * (cglbExtent->GetYHeight() / tempExtent.GetYHeight());	
	imageMatrix.multiply(agg::trans_affine_scaling(imgscalex ,imgscaley));
	glbDouble new_basex = cglbExtent->GetLeft();
	glbDouble new_basey = cglbExtent->GetBottom();
	double transx_first = (new_basex - tempExtent.GetLeft()) * tempImageW / tempExtent.GetXWidth();
	double transy_first = (new_basey - tempExtent.GetBottom()) * tempImageH / tempExtent.GetYHeight();
	imageMatrix.multiply(agg::trans_affine_translation(transx_first,transy_first));
	imageMatrix.invert();

	typedef agg::renderer_base<agg::pixfmt_rgba32> RendererBaseType;
	agg::pixfmt_rgba32 pixelFormat(rbuf);
	RendererBaseType rendererBase(pixelFormat);

	typedef agg::span_allocator<RendererBaseType::color_type> AllocatorType;
	AllocatorType allocator;

	agg::pixfmt_rgba32 pixelFormatOfData( rbuf_img );
	typedef agg::span_interpolator_linear<> InterpolatorType;
	InterpolatorType interpolator(imageMatrix);
	typedef agg::image_accessor_clone<agg::pixfmt_rgba32> ImageAccessorType;
	ImageAccessorType imageAccessor( pixelFormatOfData );
	typedef agg::image_accessor_clone<agg::pixfmt_rgba32> RGBAImageAccessorType;
	typedef agg::span_image_filter_rgba_bilinear<RGBAImageAccessorType, InterpolatorType> RGBASpanType;
	RGBASpanType generator(imageAccessor,interpolator);

	agg::rasterizer_scanline_aa<> rasterizer;
	rasterizer.reset();
	agg::conv_contour<agg::path_storage> converter(canvas);
	rasterizer.add_path(converter);
	agg::scanline_p8 scanline;

	agg::render_scanlines_aa(rasterizer,scanline,rendererBase,allocator,generator);
}