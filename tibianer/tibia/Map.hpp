#ifndef TIBIA_MAP_HPP
#define TIBIA_MAP_HPP

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include <boost/algorithm/string.hpp>
#include <boost/range/algorithm/replace_if.hpp>
#include <boost/range/algorithm/remove_if.hpp>

#include "base64.hpp"
#include "boost_zlib.hpp"

#include "tinyxml2.h"

#include "tibia/Tibia.hpp"
#include "tibia/Object.hpp"

namespace tibia
{

class Map
{

public:

    typedef std::shared_ptr<tibia::Object> ObjectPtr;
    typedef std::vector<ObjectPtr> ObjectList;

    std::vector<tibia::TileMap*> tileMapList;

    tibia::TileMap tileMapUnderGroundTiles;
    tibia::TileMap tileMapUnderGroundEdges;
    tibia::TileMap tileMapUnderGroundObjects;

    tibia::TileMap tileMapGroundTiles;
    tibia::TileMap tileMapGroundEdges;
    tibia::TileMap tileMapGroundObjects;

    tibia::TileMap tileMapAboveGroundTiles;
    tibia::TileMap tileMapAboveGroundEdges;
    tibia::TileMap tileMapAboveGroundObjects;

    bool load(std::string filename)
    {
        tinyxml2::XMLDocument doc;
        doc.LoadFile(filename.c_str());

        tinyxml2::XMLElement* docMap = doc.FirstChildElement();

        for (tinyxml2::XMLElement* docMapLayer = docMap->FirstChildElement("layer"); docMapLayer != NULL; docMapLayer = docMapLayer->NextSiblingElement("layer"))
        {
            std::string docMapLayerName = docMapLayer->Attribute("name");

            //std::cout << docMapLayerName << std::endl;

            std::string docMapLayerData = docMapLayer->FirstChildElement("data")->GetText();

            docMapLayerData.erase(boost::remove_if(docMapLayerData, boost::is_any_of(" \r\n")), docMapLayerData.end());

            docMapLayerData = base64_decode(docMapLayerData);
            docMapLayerData = boost_zlib_decompress_string_fast(docMapLayerData);

            //std::cout << docMapLayerData << std::endl;

            std::istringstream docMapLayerDataStream(docMapLayerData);
            //docMapLayerDataStream.seekg(0, std::ios::beg);

            std::vector<int> docMapLayerDataTiles;
            docMapLayerDataTiles.reserve(docMapLayerData.size() / 4);

            for (unsigned int i = 0; i < docMapLayerData.size(); i += 4)
            {
                int tileId;
                docMapLayerDataStream.read(reinterpret_cast<char*>(&tileId), 4);

                docMapLayerDataTiles.push_back(tileId);
            }

            if (docMapLayerName == "underground tiles")
            {
                tileMapUnderGroundTiles.load(docMapLayerDataTiles, docMapLayerName, tibia::TileMapTypes::tiles, tibia::ZAxis::underGround);
            }
            else if (docMapLayerName == "underground tile edges")
            {
                tileMapUnderGroundEdges.load(docMapLayerDataTiles, docMapLayerName, tibia::TileMapTypes::edges, tibia::ZAxis::underGround);
            }
            else if (docMapLayerName == "underground tile objects")
            {
                tileMapUnderGroundObjects.load(docMapLayerDataTiles, docMapLayerName, tibia::TileMapTypes::objects, tibia::ZAxis::underGround);
            }

            else if (docMapLayerName == "ground tiles")
            {
                tileMapGroundTiles.load(docMapLayerDataTiles, docMapLayerName, tibia::TileMapTypes::tiles, tibia::ZAxis::ground);
            }
            else if (docMapLayerName == "ground tile edges")
            {
                tileMapGroundEdges.load(docMapLayerDataTiles, docMapLayerName, tibia::TileMapTypes::edges, tibia::ZAxis::ground);
            }
            else if (docMapLayerName == "ground tile objects")
            {
                tileMapGroundObjects.load(docMapLayerDataTiles, docMapLayerName, tibia::TileMapTypes::objects, tibia::ZAxis::ground);
            }

            else if (docMapLayerName == "aboveground tiles")
            {
                tileMapAboveGroundTiles.load(docMapLayerDataTiles, docMapLayerName, tibia::TileMapTypes::tiles, tibia::ZAxis::aboveGround);
            }
            else if (docMapLayerName == "aboveground tile edges")
            {
                tileMapAboveGroundEdges.load(docMapLayerDataTiles, docMapLayerName, tibia::TileMapTypes::edges, tibia::ZAxis::aboveGround);
            }
            else if (docMapLayerName == "aboveground tile objects")
            {
                tileMapAboveGroundObjects.load(docMapLayerDataTiles, docMapLayerName, tibia::TileMapTypes::objects, tibia::ZAxis::aboveGround);
            }
        }

        tileMapList.push_back(&tileMapUnderGroundTiles);
        tileMapList.push_back(&tileMapUnderGroundEdges);
        tileMapList.push_back(&tileMapUnderGroundObjects);

        tileMapList.push_back(&tileMapGroundTiles);
        tileMapList.push_back(&tileMapGroundEdges);
        tileMapList.push_back(&tileMapGroundObjects);

        tileMapList.push_back(&tileMapAboveGroundTiles);
        tileMapList.push_back(&tileMapAboveGroundEdges);
        tileMapList.push_back(&tileMapAboveGroundObjects);

        for (tinyxml2::XMLElement* docMapObjectGroup = docMap->FirstChildElement("objectgroup"); docMapObjectGroup != NULL; docMapObjectGroup = docMapObjectGroup->NextSiblingElement("objectgroup"))
        {
            std::string docMapObjectGroupName = docMapObjectGroup->Attribute("name");

            //std::cout << docMapObjectGroupName << std::endl;

            std::size_t findObjectsClass = docMapObjectGroupName.find("objects");

            if (findObjectsClass == std::string::npos)
            {
                continue;
            }

            int docMapObjectZ = tibia::ZAxis::ground;

            if (docMapObjectGroupName == "ground objects")
            {
                docMapObjectZ = tibia::ZAxis::ground;
            }
            else if (docMapObjectGroupName == "underground objects")
            {
                docMapObjectZ = tibia::ZAxis::underGround;
            }
            else if (docMapObjectGroupName == "aboveground objects")
            {
                docMapObjectZ = tibia::ZAxis::aboveGround;
            }

            for (tinyxml2::XMLElement* docMapObject = docMapObjectGroup->FirstChildElement("object"); docMapObject != NULL; docMapObject = docMapObject->NextSiblingElement("object"))
            {
                int docMapObjectId = docMapObject->IntAttribute("gid");

                //std::cout << docMapObjectId << std::endl;

                int docMapObjectTileX = docMapObject->IntAttribute("x");
                int docMapObjectTileY = docMapObject->IntAttribute("y") - tibia::TILE_SIZE; // y-axis bug for objects in Tiled editor?

                ObjectPtr object = std::make_shared<tibia::Object>(docMapObjectTileX, docMapObjectTileY, docMapObjectZ, docMapObjectId);
                m_objectsList.push_back(object);
            }
        }

        return true;
    }

    ObjectList* getObjectsList()
    {
        return &m_objectsList;
    }

private:

    ObjectList m_objectsList;

};

}

#endif // TIBIA_MAP_HPP
