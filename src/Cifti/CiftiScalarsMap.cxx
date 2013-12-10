/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*LICENSE_END*/

#include "CiftiScalarsMap.h"

#include "CaretAssert.h"
#include "CaretException.h"
#include "CaretLogger.h"
//HACK: to compare metadata in a const function, we make a copy and remove the palette data - but metadata's copy intentionally breaks == because of the UUID, so we need to reset it
#include "GiftiMetaDataXmlElements.h"

using namespace caret;

void CiftiScalarsMap::clear()
{
    m_maps.clear();
}

GiftiMetaData* CiftiScalarsMap::getMapMetadata(const int64_t& index) const
{
    CaretAssertVectorIndex(m_maps, index);
    return &(m_maps[index].m_metaData);
}

const AString& CiftiScalarsMap::getMapName(const int64_t& index) const
{
    CaretAssertVectorIndex(m_maps, index);
    return m_maps[index].m_name;
}

PaletteColorMapping* CiftiScalarsMap::getMapPalette(const int64_t& index) const
{
    CaretAssertVectorIndex(m_maps, index);
    return m_maps[index].getPalette();
}

PaletteColorMapping* CiftiScalarsMap::ScalarMap::getPalette() const
{
    if (m_palette != NULL)
    {
        return m_palette;
    }
    m_palette.grabNew(new PaletteColorMapping());
    if (m_metaData.exists("PaletteColorMapping"))
    {
        try
        {
            m_palette->decodeFromStringXML(m_metaData.get("PaletteColorMapping"));
        } catch (XmlException& e) {
            CaretLogWarning("failed to parse palette settings from metadata: " + e.whatString());
        }
    }
    return m_palette;
}

void CiftiScalarsMap::setLength(const int64_t& length)
{
    CaretAssert(length > 0);
    m_maps.resize(length);
}

void CiftiScalarsMap::setMapName(const int64_t& index, const AString& mapName)
{
    CaretAssertVectorIndex(m_maps, index);
    m_maps[index].m_name = mapName;
}

bool CiftiScalarsMap::operator==(const CiftiIndexMap& rhs) const
{
    if (rhs.getType() != getType()) return false;
    const CiftiScalarsMap& myrhs = dynamic_cast<const CiftiScalarsMap&>(rhs);
    return (m_maps == myrhs.m_maps);
}

bool CiftiScalarsMap::ScalarMap::operator==(const CiftiScalarsMap::ScalarMap& rhs) const
{
    if (m_name != rhs.m_name) return false;
    if (*(getPalette()) != *(rhs.getPalette())) return false;
    GiftiMetaData mytemp = m_metaData, rhstemp = rhs.m_metaData;
    mytemp.remove("PaletteColorMapping");//we already compared the true palettes, so don't compare the metadata that may or may not encode them
    if (m_metaData.exists(GiftiMetaDataXmlElements::METADATA_NAME_UNIQUE_ID))//HACK: fix the copy-breaks-UUID silliness
    {
        mytemp.set(GiftiMetaDataXmlElements::METADATA_NAME_UNIQUE_ID, m_metaData.get(GiftiMetaDataXmlElements::METADATA_NAME_UNIQUE_ID));
    }
    rhstemp.remove("PaletteColorMapping");
    if (rhs.m_metaData.exists(GiftiMetaDataXmlElements::METADATA_NAME_UNIQUE_ID))
    {
        rhstemp.set(GiftiMetaDataXmlElements::METADATA_NAME_UNIQUE_ID, rhs.m_metaData.get(GiftiMetaDataXmlElements::METADATA_NAME_UNIQUE_ID));
    }
    return (mytemp == rhstemp);
}

void CiftiScalarsMap::readXML1(QXmlStreamReader& xml)
{
    CaretLogFiner("parsing nonstandard scalars mapping type in cifti-1");
    clear();
    for (xml.readNext(); !xml.atEnd() && !xml.isEndElement(); xml.readNext())
    {
        switch (xml.tokenType())
        {
            case QXmlStreamReader::StartElement:
            {
                if (xml.name() != "NamedMap")
                {
                    throw CaretException("unexpected element in scalars mapping type: " + xml.name().toString());
                }
                m_maps.push_back(ScalarMap());//HACK: because operator= is deliberately broken by GiftiMetadata for UUID
                m_maps.back().readXML1(xml);
                if (xml.hasError()) return;
                break;
            }
            default:
                break;
        }
    }
    CaretAssert(xml.isEndElement() && xml.name() == "MatrixIndicesMap");
}

void CiftiScalarsMap::readXML2(QXmlStreamReader& xml)
{
    clear();
    for (xml.readNext(); !xml.atEnd() && !xml.isEndElement(); xml.readNext())
    {
        switch (xml.tokenType())
        {
            case QXmlStreamReader::StartElement:
            {
                if (xml.name() != "NamedMap")
                {
                    throw CaretException("unexpected element in scalars mapping type: " + xml.name().toString());
                }
                m_maps.push_back(ScalarMap());//HACK: because operator= is deliberately broken by GiftiMetadata for UUID
                m_maps.back().readXML2(xml);
                if (xml.hasError()) return;
                break;
            }
            default:
                break;
        }
    }
    CaretAssert(xml.isEndElement() && xml.name() == "MatrixIndicesMap");
}

void CiftiScalarsMap::ScalarMap::readXML1(QXmlStreamReader& xml)
{
    bool haveName = false, haveMetaData = false;
    for (xml.readNext(); !xml.atEnd() && !xml.isEndElement(); xml.readNext())
    {
        switch (xml.tokenType())
        {
            case QXmlStreamReader::StartElement:
            {
                QStringRef name = xml.name();
                if (name == "MetaData")
                {
                    if (haveMetaData)
                    {
                        throw CaretException("MetaData specified multiple times in one NamedMap");
                    }
                    m_metaData.readCiftiXML1(xml);
                    if (xml.hasError()) return;
                    haveMetaData = true;
                } else if (name == "MapName") {
                    if (haveName)
                    {
                        throw CaretException("MapName specified multiple times in one NamedMap");
                    }
                    m_name = xml.readElementText();//raises error if element encountered
                    if (xml.hasError()) return;
                    haveName = true;
                } else {
                    throw CaretException("unexpected element in NamedMap: " + name.toString());
                }
                break;
            }
            default:
                break;
        }
    }
    if (!haveName)
    {
        throw CaretException("NamedMap missing required child element MapName");
    }
    CaretAssert(xml.isEndElement() && xml.name() == "NamedMap");
}

void CiftiScalarsMap::ScalarMap::readXML2(QXmlStreamReader& xml)
{
    bool haveName = false, haveMetaData = false;
    for (xml.readNext(); !xml.atEnd() && !xml.isEndElement(); xml.readNext())
    {
        switch (xml.tokenType())
        {
            case QXmlStreamReader::StartElement:
            {
                QStringRef name = xml.name();
                if (name == "MetaData")
                {
                    if (haveMetaData)
                    {
                        throw CaretException("MetaData specified multiple times in one NamedMap");
                    }
                    m_metaData.readCiftiXML2(xml);
                    if (xml.hasError()) return;
                    haveMetaData = true;
                } else if (name == "MapName") {
                    if (haveName)
                    {
                        throw CaretException("MapName specified multiple times in one NamedMap");
                    }
                    m_name = xml.readElementText();//raises error if element encountered
                    if (xml.hasError()) return;
                    haveName = true;
                } else {
                    throw CaretException("unexpected element in NamedMap: " + name.toString());
                }
                break;
            }
            default:
                break;
        }
    }
    if (!haveName)
    {
        throw CaretException("NamedMap missing required child element MapName");
    }
    CaretAssert(xml.isEndElement() && xml.name() == "NamedMap");
}

void CiftiScalarsMap::writeXML1(QXmlStreamWriter& xml) const
{
    CaretLogFiner("writing nonstandard scalars mapping type in cifti-1");
    xml.writeAttribute("IndicesMapToDataType", "CIFTI_INDEX_TYPE_SCALARS");
    int64_t numMaps = (int64_t)m_maps.size();
    for (int64_t i = 0; i < numMaps; ++i)
    {
        xml.writeStartElement("NamedMap");
        xml.writeTextElement("MapName", m_maps[i].m_name);
        if (m_maps[i].m_palette != NULL)
        {
            m_maps[i].m_metaData.set("PaletteColorMapping", m_maps[i].m_palette->encodeInXML());
        }
        m_maps[i].m_metaData.writeCiftiXML1(xml);
        xml.writeEndElement();
    }
}

void CiftiScalarsMap::writeXML2(QXmlStreamWriter& xml) const
{
    int64_t numMaps = (int64_t)m_maps.size();
    xml.writeAttribute("IndicesMapToDataType", "CIFTI_INDEX_TYPE_SCALARS");
    for (int64_t i = 0; i < numMaps; ++i)
    {
        xml.writeStartElement("NamedMap");
        xml.writeTextElement("MapName", m_maps[i].m_name);
        if (m_maps[i].m_palette != NULL)
        {
            m_maps[i].m_metaData.set("PaletteColorMapping", m_maps[i].m_palette->encodeInXML());
        }
        m_maps[i].m_metaData.writeCiftiXML1(xml);
        xml.writeEndElement();
    }
}
