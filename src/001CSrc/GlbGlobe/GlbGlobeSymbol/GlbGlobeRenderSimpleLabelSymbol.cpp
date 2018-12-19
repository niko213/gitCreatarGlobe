#include "StdAfx.h"
#include "GlbGlobeRenderSimpleLabelSymbol.h"
#include "osg/Billboard"
#include "osg/Texture2D"
#include "osgDB/ReadFile"
#include "osg/BlendColor"
#include "osg/BlendFunc"
#include "osg/TexEnv"
#include "osg/Material"
#include "GlbPoint.h"

using namespace GlbGlobe;
#define WIDTHBYTES(bites) (((bites) + 31) / 32 * 4);

CGlbGlobeRenderSimpleLabelSymbol::CGlbGlobeRenderSimpleLabelSymbol(void)
{
	mpr_font = new agg::font_engine_win32_tt_int16(::GetDC(::GetActiveWindow()));
	mpr_font->flip_y(true); //上下翻转
	mpr_font->hinting(true); //字体修正
	mpr_font->create_font("宋体",agg::glyph_ren_agg_gray8);
}

CGlbGlobeRenderSimpleLabelSymbol::~CGlbGlobeRenderSimpleLabelSymbol(void)
{
	if(mpr_font)
		delete mpr_font;
	mpr_font = NULL;
}

osg::Node * CGlbGlobeRenderSimpleLabelSymbol::Draw( GlbRenderSimpleLabel* info,IGlbGeometry *geo,
	CGlbFeature *feature,CGlbGlobe *globe )
{
	if(info == NULL || geo == NULL || globe == NULL)
		return NULL;

	osg::Switch *swiNode = new osg::Switch;
	swiNode->setName("simpleLabel");
	int sizeWidth = 0,sizeHeight = 0;
	osg::ref_ptr<osg::Image> tempImage = NULL;

	glbBool isEmpty = true;
	glbInt32 textSize = 0;
	std::vector<CGlbWString> wsRenderContainer;
	glbDouble bytes = 0.0;//记录对齐方式参考字节数

	CGlbWString content = L"";
	if(info->textInfo && info->textInfo->size)
		textSize = info->textInfo->size->GetValue(feature);
	if(info->textInfo && info->textInfo->content)
		content = info->textInfo->content->GetValue(feature);

	glbInt32 textCount = content.size();
	if(textCount != 0)
		isEmpty = false;

	//由于解决后期发现的设置加粗、倾斜会出问题，
	//把设置这两步的代码写到这里，具体原因没分析出来为什么
	if(!isEmpty && info->textInfo)
		wsRenderContainer = DealTextContent(info->textInfo,feature,bytes);

	if(isEmpty == false)
	{
		CGlbWString textFont = L"宋体";
		glbBool isItalic = false;
		if(info->textInfo && info->textInfo->font)
			textFont = info->textInfo->font->GetValue(feature);//字体
		if(info->textInfo && info->textInfo->isItalic)
			isItalic = info->textInfo->isItalic->GetValue(feature);
		DealComputeBufferSize(wsRenderContainer.size(),textSize,textFont,
			content,bytes,isItalic,sizeWidth,sizeHeight);
	}

	if(sizeWidth == 0 || sizeHeight == 0)
		return NULL;

	glbBool isLimit = false;
	glbDouble ratio = 1.0;
	if(info->geoInfo->isLimit)
		isLimit = info->geoInfo->isLimit->GetValue(feature);
	if(info->geoInfo->ratio)
		ratio = info->geoInfo->ratio->GetValue(feature);

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
	if(info->textInfo->backColor)
		textbackColor = info->textInfo->backColor->GetValue(feature);
	if(info->textInfo->backOpacity)
		textbackOpacity = info->textInfo->backOpacity->GetValue(feature);

	if(0xFFFFFFFF != textbackColor && textbackOpacity != 0)
	{
		osg::Vec4 backColor = GetColor(textbackColor);
		renb.clear(agg::rgba(backColor.x(), backColor.y(), backColor.z(),textbackOpacity / 100.0));
	}

	//ceshi
	//renb.clear(agg::rgba(1.0,1.0,1.0,1.0));

	if(info && info->textInfo)
		DealRenderText(isEmpty,wsRenderContainer,content,textSize,info->textInfo,sizeHeight,renb,feature);

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

	ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	if (image)
	{
		texture->setImage(image);
		stateset->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
	}

	osg::ref_ptr<GlbGlobeAutoTransform> autoTransform = new GlbGlobeAutoTransform(globe);
	GlbGlobeBillboardModeEnum mode = GLB_BILLBOARD_SCREEN;
	if(info->mode)
	{
		mode = (GlbGlobeBillboardModeEnum)info->mode->GetValue(feature);
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
	if(info->labelAlign)
		positionAlign = (GlbGlobeLabelAlignTypeEnum)info->labelAlign->GetValue(feature);

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

	osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material *>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
	if(material == NULL)
		material = new osg::Material;
	material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(1,1,1,1));
	material->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(1,1,1,1));
	material->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(1,1,1,1));
	material->setSpecular(osg::Material::FRONT_AND_BACK,osg::Vec4(1,1,1,1));
	material->setAlpha(osg::Material::FRONT_AND_BACK,1.0);
	stateset->setAttribute(material,osg::StateAttribute::ON);
	//透明度
	osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::ONE/*SRC_ALPHA*/,osg::BlendFunc::ONE_MINUS_SRC_ALPHA);	//设置混合方程
	stateset->setAttributeAndModes(pBlendFun.get(),osg::StateAttribute::ON);
	stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
	stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON);
	stateset->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
	stateset->setMode( GL_NORMALIZE, osg::StateAttribute::ON );
	osg::ref_ptr<osg::TexEnv> texenv=new osg::TexEnv;
	texenv->setMode(osg::TexEnv::MODULATE);
	//设置纹理环境
	stateset->setTextureAttribute(0,texenv.get());
	labelGeom->setStateSet(stateset);

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(labelGeom);
	autoTransform->addChild(geode);
	osg::MatrixTransform *mt = new osg::MatrixTransform;
	osg::Matrixd m;
	CGlbPoint *geoTmp=dynamic_cast<CGlbPoint *>(geo);
	double xx = 0.0,yy = 0.0,zz = 0.0,
		xOffset = 0.0,yOffset = 0.0,zOffset = 0.0;
	if(info->xOffset && info->yOffset && info->zOffset)
	{
		xOffset = info->xOffset->GetValue(feature);
		yOffset = info->yOffset->GetValue(feature);
		zOffset = info->zOffset->GetValue(feature);
	}
	geoTmp->GetXYZ(&xx,&yy,&zz);
	m.makeTranslate(xx + xOffset,yy + yOffset,zz + zOffset);
	mt->setMatrix(m);
	mt->addChild(autoTransform);
	swiNode->addChild(mt);
	return swiNode;
}

void CGlbGlobeRenderSimpleLabelSymbol::DealText(osg::Vec2d leftUp,CGlbWString content,
	renb_type &renb,GlbRenderTextInfo *textInfo,CGlbFeature *feature)
{
	if(textInfo == NULL || content.size() == 0)
		return;

	glbInt32 textFontColor = 0x00000000;
	if(textInfo->color)
		textFontColor = textInfo->color->GetValue(feature);

	glbBool isUnderline = false;
	if(textInfo->isUnderline)
		isUnderline = textInfo->isUnderline->GetValue(feature);//下划线

	agg::rasterizer_scanline_aa<> ras;  
	agg::scanline_p8 sl;

	agg::font_cache_manager<agg::font_engine_win32_tt_int16> *fontManager = 
		new agg::font_cache_manager<agg::font_engine_win32_tt_int16>(*mpr_font);

	osg::Vec4 textColor = GetColor(textFontColor);
	//根据图片与文字的轴模式设置起始位置
	double x = leftUp.x(),y = leftUp.y();
	double beginX = x,beginY = y;
	const wchar_t *text = content.c_str();
	bool glyphAvailable = false;

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

std::vector<CGlbWString> GlbGlobe::CGlbGlobeRenderSimpleLabelSymbol::DealTextContent(
	GlbRenderTextInfo *textRenderInfo,CGlbFeature *feature,glbDouble &bytes)
{
	std::vector<CGlbWString> wsRenderContainer;
	if(!textRenderInfo || !textRenderInfo->font)
		return wsRenderContainer;

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
		if (wchar == '\n')
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
	if(textRenderInfo->multilineAlign)
		multilineAlign = (GlbGlobeMultilineAlignTypeEnum)textRenderInfo->multilineAlign->GetValue(feature);


	glbDouble bytes = 0.0;//记录对齐方式参考字节数
	CGlbWString textFont = textRenderInfo->font->GetValue(feature);//字体
	mpr_font->height(textSize);
	mpr_font->width(0);
	if(textRenderInfo && textRenderInfo->font)
	{
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
	}
	mpr_font->create_font(textFont.ToString().c_str(),agg::glyph_ren_agg_gray8);
	agg::font_cache_manager<agg::font_engine_win32_tt_int16> *fontManager = new agg::font_cache_manager<agg::font_engine_win32_tt_int16>(*mpr_font);
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

void GlbGlobe::CGlbGlobeRenderSimpleLabelSymbol::DealComputeBufferSize( 
	glbInt32 multilineSize,glbInt32 textSize,CGlbWString textFont,
	CGlbWString content,glbInt32 bytes,glbBool isItalic,
	glbInt32 &sizeWidth,glbInt32 &sizeHeight)
{
	glbDouble simplelineX = 0.0,simplelineY = 0.0;
	if(multilineSize == 0)//多行里没有文本，说明只有一行文本
	{
		mpr_font->create_font(textFont.ToString().c_str(),agg::glyph_ren_agg_gray8);
		mpr_font->height(textSize);
		mpr_font->width(0);
		agg::font_cache_manager<agg::font_engine_win32_tt_int16> *fontManager = 
			new agg::font_cache_manager<agg::font_engine_win32_tt_int16>(*mpr_font);
		const wchar_t *text = content.c_str();
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

	if (multilineSize > 0)
	{
		sizeWidth = bytes + textSize;
		sizeHeight = textSize * multilineSize + textSize;
	}
	else
	{
		sizeWidth = simplelineX + textSize / 5;//加粗和倾斜的时候最后一个字会部分缺失
		sizeHeight = textSize + textSize / 5/* * 2*/;//chenpeng0822
	}
}

void GlbGlobe::CGlbGlobeRenderSimpleLabelSymbol::DealRenderText(
	glbBool isEmpty,std::vector<CGlbWString> wsRenderContainer,
	CGlbWString content,glbInt32 textSize,GlbRenderTextInfo *textInfo,
	glbInt32 sizeHeight,renb_type &renb,CGlbFeature *feature)
{
	if(isEmpty == false)
	{
		glbFloat x = 0.0, y = 0.0;
		glbBool isItalic = false;
		if(textInfo && textInfo->isItalic && textInfo->isItalic->GetValue(feature))
			isItalic = true;
		if (wsRenderContainer.size() > 0)
		{
			for (int i = 0; i < (int)wsRenderContainer.size();++i)
				DealText(osg::Vec2(0 , sizeHeight - i * textSize - textSize),wsRenderContainer.at((int)wsRenderContainer.size() - i - 1),renb,textInfo,feature);
		}
		else
		{
			DealText(osg::Vec2(0.0,sizeHeight - textSize / 5),content,renb,textInfo,feature);
		}
	}
}