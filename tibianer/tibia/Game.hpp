#ifndef TIBIA_GAME_HPP
#define TIBIA_GAME_HPP

#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include <boost/algorithm/string.hpp>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "tibia/Tibia.hpp"
#include "tibia/Tile.hpp"
#include "tibia/TileMap.hpp"
#include "tibia/Map.hpp"
#include "tibia/Thing.hpp"
#include "tibia/Object.hpp"
#include "tibia/Creature.hpp"
#include "tibia/Animation.hpp"
#include "tibia/Projectile.hpp"

namespace tibia
{

class Game
{

public:

    typedef std::shared_ptr<tibia::Thing>      ThingPtr;
    typedef std::shared_ptr<tibia::Creature>   CreaturePtr;
    typedef std::shared_ptr<tibia::Animation>  AnimationPtr;
    typedef std::shared_ptr<tibia::Projectile> ProjectilePtr;

    typedef std::shared_ptr<sf::Sound> SoundPtr;

    Game()
    :
        m_windowView(sf::FloatRect(0, 0, tibia::GuiData::gameWindowWidth, tibia::GuiData::gameWindowHeight)),
        m_miniMapWindowView(sf::FloatRect(0, 0, tibia::GuiData::gameWindowWidth * 2, tibia::GuiData::gameWindowHeight * 2))
    {
        m_tileVertices.setPrimitiveType(sf::Quads);

        m_rtLight.create(MAP_TILE_XY_MAX, MAP_TILE_XY_MAX);

        m_rectLight.setPosition(0, 0);
        m_rectLight.setSize(sf::Vector2f(MAP_TILE_XY_MAX, MAP_TILE_XY_MAX));

        CreaturePtr player = std::make_shared<tibia::Creature>(0, 0, tibia::ZAxis::ground);
        player->setName("Player");
        player->setIsPlayer(true);
        player->setHasOutfit(true);
        player->setIsSitting(false);
        player->setTeam(tibia::Teams::good);
        player->setHpMax(10000);
        player->setHp(10000);
        player->setMovementSpeed(tibia::MovementSpeeds::player);

        m_player = player;

        spawnCreature(m_player);
    }

    bool createWindows()
    {
        if (m_window.create(tibia::GuiData::gameWindowWidth, tibia::GuiData::gameWindowHeight) == false)
        {
            return false;
        }

        m_windowBorder.setSize(sf::Vector2f(m_window.getSize().x, m_window.getSize().y));
        m_windowBorder.setOutlineColor(tibia::Colors::windowBorderColor);
        m_windowBorder.setOutlineThickness(1);
        m_windowBorder.setPosition(tibia::GuiData::gameWindowX, tibia::GuiData::gameWindowY);

        if (m_miniMapWindow.create(tibia::GuiData::miniMapWindowWidth, tibia::GuiData::miniMapWindowHeight) == false)
        {
            return false;
        }

        m_miniMapWindowBorder.setSize(sf::Vector2f(m_miniMapWindow.getSize().x, m_miniMapWindow.getSize().y));
        m_miniMapWindowBorder.setOutlineColor(tibia::Colors::windowBorderColor);
        m_miniMapWindowBorder.setOutlineThickness(1);
        m_miniMapWindowBorder.setPosition(tibia::GuiData::miniMapWindowX, tibia::GuiData::miniMapWindowY);

        return true;
    }

    bool loadTextures()
    {
        if (tibia::Textures::sprites.loadFromFile("images/sprites.png") == false)
        {
            return false;
        }

        if (tibia::Textures::font.loadFromFile("images/font.png") == false)
        {
            return false;
        }

        if (tibia::Textures::font2.loadFromFile("images/font2.png") == false)
        {
            return false;
        }

        if (tibia::Textures::light.loadFromFile("images/light.png") == false)
        {
            return false;
        }

        if (tibia::Textures::light2.loadFromFile("images/light2.png") == false)
        {
            return false;
        }

        if (tibia::Textures::light3.loadFromFile("images/light3.png") == false)
        {
            return false;
        }

        if (tibia::Textures::light4.loadFromFile("images/light4.png") == false)
        {
            return false;
        }

        if (tibia::Textures::light5.loadFromFile("images/light5.png") == false)
        {
            return false;
        }

        return true;
    }

    bool loadFonts()
    {
        if (m_font.loadFromFile(tibia::Fonts::default) == false)
        {
            return false;
        }

        if (m_fontSmall.loadFromFile(tibia::Fonts::small) == false)
        {
            return false;
        }

        if (m_fontMartel.loadFromFile(tibia::Fonts::martel) == false)
        {
            return false;
        }

        return true;
    }

    bool loadSounds()
    {
        //if (tibia::Sounds::death.loadFromFile("sounds/everquest/snd1/death_m.wav") == false)
        //{
            //return false;
        //}

        return true;
    }

    bool loadMap(std::string filename)
    {
        return m_map.load(filename);
    }

    void loadCreatures()
    {
        //
    }

    void loadObjects()
    {
        m_objectsList = *m_map.getObjectsList();
    }

    void loadSpriteFlags()
    {
        for (unsigned int i = 1; i < tibia::SPRITES_TOTAL + 1; i++)
        {
            tibia::spriteFlags[i] = tibia::getSpriteFlags(i);
        }
    }

    void handleKeyboardInput()
    {
        //
    }

    void handleMouseInput()
    {
        //
    }

    bool handleCreatureDamage(tibia::Creature* attacker, tibia::Creature* defender, int damage, int* animationOnHit, int* animatedDecalOnHit, int* animatedDecalOnKill)
    {
        if (attacker == nullptr || defender == nullptr)
        {
            return false;
        }

        if (attacker == defender)
        {
            return false;
        }

        if (attacker->getTeam() == defender->getTeam())
        {
            return false;
        }

        if (defender->getTeam() == tibia::Teams::neutral)
        {
            return false;
        }

        defender->takeDamageFromCreature(attacker, damage);

        spawnAnimation
        (
            defender->getTileX(),
            defender->getTileY(),
            defender->getZ(),
            animationOnHit
        );

        std::cout
            << attacker->getName()
            << " hit "
            << defender->getName()
            << " for "
            << damage
            << " damage."
            << std::endl;

        if (defender->isDead() == false)
        {
            spawnAnimatedDecal
            (
                defender->getTileX(),
                defender->getTileY(),
                defender->getZ(),
                animatedDecalOnHit,
                tibia::AnimationTimes::decal
            );
        }
        else
        {
            //SoundPtr sound = std::make_shared<sf::Sound>();
            //sound->setBuffer(tibia::Sounds::death);
            //sound->setVolume(tibia::calculateVolumeByDistance(creature->getDistanceFromPlayer()));
            //spawnSound(sound);

            spawnAnimatedDecal
            (
                defender->getTileX(),
                defender->getTileY(),
                defender->getZ(),
                animatedDecalOnKill,
                tibia::AnimationTimes::decal
            );

            if (defender->isPlayer() == true)
            {
                showGameText("You are dead.", tibia::FontSizes::game, tibia::Colors::white);
            }

            std::cout
                << attacker->getName()
                << " killed "
                << defender->getName()
                << std::endl;
        }

        return true;
    }

    bool handleCreatureMovement(tibia::Creature* creature, int direction, bool turnOnly = false)
    {
        bool collisionOccured = false;

        if (creature->isDead() == true)
        {
            return true;
        }

        if (turnOnly == false)
        {
            if (checkMovementCollision(creature, direction) == false)
            {
                if (checkMovementMoveAboveOrBelow(creature, direction) == true)
                {
                    return false;
                }

                checkMovementStepTile(creature, direction, true);

                creature->doMove(direction);

                checkMovementStepTile(creature, direction, false);
            }
            else
            {
                collisionOccured == true;
            }
        }

        creature->doTurn(direction);

        creature->setIsSitting(checkCreatureIsSitting(creature));

        return collisionOccured;
    }

    void doCreatureLogic()
    {
        for (auto creature : m_creaturesList)
        {
            if (creature->isPlayer() == true)
            {
                continue;
            }

            if (creature->isDead() == true)
            {
                continue;
            }

            int random = 0;

            if (m_creaturesList.size() > tibia::CREATURES_MAX_LOAD)
            {
                random = tibia::getRandomNumber(1, 100);

                if (random > 50)
                {
                    continue;
                }
            }

            random = tibia::getRandomNumber(1, 100);

            if (random > 50)
            {
                int direction = creature->getDirection();

                if (random > 75)
                {
                    direction = tibia::getRandomNumber(tibia::Directions::up, tibia::Directions::left);
                }

                if (handleCreatureMovement(creature.get(), direction) == false)
                {
                    if (random > 90)
                    {
                        creature->doTurn(direction);
                    }
                }
            }
            else
            {
                if (m_creaturesList.size() > tibia::CREATURES_MAX_LOAD)
                {
                    if (creature->getDistanceFromPlayer() > tibia::DRAW_DISTANCE_MAX)
                    {
                        continue;
                    }
                }

                for (auto findCreature : m_creaturesList)
                {
                    if (findCreature == creature)
                    {
                        continue;
                    }

                    if (findCreature->isDead() == true)
                    {
                        continue;
                    }

                    if (findCreature->getZ() != creature->getZ())
                    {
                        continue;
                    }

                    if (findCreature->getTeam() == creature->getTeam() || findCreature->getTeam() == tibia::Teams::neutral)
                    {
                        continue;
                    }

                    sf::Vector2f origin;
                    origin.x = creature->getTileX();
                    origin.y = creature->getTileY();

                    sf::Vector2f destination;
                    destination.x = findCreature->getTileX();
                    destination.y = findCreature->getTileY();

                    sf::Vector2f normal = tibia::getNormalByVectors(origin, destination);

                    normal.x = tibia::getFloatNormalEx(normal.x);
                    normal.y = tibia::getFloatNormalEx(normal.y);

                    int direction = tibia::getDirectionByVector(normal);

                    if (random < 25)
                    {
                        if (calculateDistanceBetweenCreatures(creature.get(), findCreature.get()) > tibia::ProjectileRanges::default)
                        {
                            continue;
                        }

                        sf::Vector2u checkBlockProjectilesPosition = getTileCoordsByCreatureDirection(creature.get(), direction);

                        if (checkTileIsBlockProjectiles(checkBlockProjectilesPosition, creature->getZ()) == true)
                        {
                            continue;
                        }

                        int projectileType = tibia::ProjectileTypes::spellFire;

                        switch (creature->getTeam())
                        {
                            case tibia::Teams::good:
                                projectileType = tibia::ProjectileTypes::spellBlue;
                                break;

                            case tibia::Teams::evil:
                                projectileType = tibia::ProjectileTypes::spellBlack;
                                break;
                        }

                        bool projectileIsDiagnonal = true;

                        if (findCreature->getTileX() == creature->getTileX() || findCreature->getTileY() == creature->getTileY())
                        {
                            projectileIsDiagnonal = false;
                        }

                        bool creatureShouldShootProjectile = true;

                        int random3 = tibia::getRandomNumber(1, 100);

                        if (random3 > 10 && projectileIsDiagnonal == true)
                        {
                            creatureShouldShootProjectile = false;
                        }

                        if (creatureShouldShootProjectile == true)
                        {
                            spawnProjectile
                            (
                                creature.get(),
                                projectileType,
                                direction,
                                origin,
                                destination
                            );
                        }

                        creature->doTurn(direction);

                        break;
                    }
                    else
                    {
                        int random2 = tibia::getRandomNumber(1, 100);

                        if (random2 > 10)
                        {
                            continue;
                        }

                        handleCreatureMovement(creature.get(), direction);

                        creature->doTurn(direction);

                        break;
                    }
                }
            }

            random = tibia::getRandomNumber(1, 100);

            if (random > 10)
            {
                doCreatureUseLadder(creature.get(), creature->getTilePosition());
            }

            creature->update();
        }
    }

    void updatePlayer()
    {
        m_player->update();
    }

    void updateCreatures()
    {
        for (auto creature : m_creaturesList)
        {
            if (creature->isPlayer() == true)
            {
                continue;
            }

            float distanceFromPlayer = calculateDistanceBetweenCreatures(m_player.get(), creature.get());

            creature->setDistanceFromPlayer(distanceFromPlayer);

            creature->update();
        }
    }

    void updateObjects()
    {
        for (auto object : m_objectsList)
        {
            object->update();
        }
    }

    void updateAnimations()
    {
        for (auto animation : m_animationsList)
        {
            animation->update();
        }
    }

    void updateAnimatedDecals()
    {
        for (auto animatedDecal : m_animatedDecalsList)
        {
            animatedDecal->update();
        }
    }

    void updateProjectiles()
    {
        for (auto projectile : m_projectilesList)
        {
            projectile->update();
        }
    }

    void updateSounds()
    {
        for (auto soundsList_it = m_soundsList.begin(); soundsList_it != m_soundsList.end(); soundsList_it++)
        {
            sf::Sound* sound = soundsList_it->get();

            if (sound == nullptr)
            {
                return;
            }

            if (sound->getStatus() == sf::SoundSource::Status::Stopped)
            {
                soundsList_it = m_soundsList.erase(soundsList_it);
                soundsList_it--;
                continue;
            }
        }
    }

    void spawnSound(SoundPtr sound)
    {
        sound->play();

        m_soundsList.push_back(sound);
    }

    void spawnCreature(CreaturePtr creature)
    {
        m_creaturesSpawnList.push_back(creature);
    }

    void spawnAnimation(int tileX, int tileY, int z, int animationId[], float frameTime = tibia::AnimationTimes::default)
    {
        if (m_player->getZ() == tibia::ZAxis::underGround && z != tibia::ZAxis::underGround)
        {
            return;
        }

        AnimationPtr animation = std::make_shared<tibia::Animation>(tileX, tileY, z, animationId[0], animationId[1]);
        animation->setFrameTime(frameTime);

        if
        (
            animationId[0] == tibia::Animations::fire[0]        ||
            animationId[0] == tibia::Animations::electricity[0]
        )
        {
            animation->setNumRepeat(1);
        }

        m_animationsSpawnList.push_back(animation);
    }

    void spawnAnimatedDecal(int tileX, int tileY, int z, int animationId[], float frameTime = tibia::AnimationTimes::decal)
    {
        if (m_animatedDecalsList.size() != 0)
        {
            for (auto animatedDecalsList_it = m_animatedDecalsList.begin(); animatedDecalsList_it != m_animatedDecalsList.end(); animatedDecalsList_it++)
            {
                tibia::Animation* ad = animatedDecalsList_it->get();

                if (ad == nullptr)
                {
                    return;
                }

                if (ad->getId() == animationId[0] && ad->getTileX() == tileX && ad->getTileY() == tileY && ad->getZ() == z)
                {
                    return;

                    //animatedDecalsList_it = m_animatedDecalsList.erase(animatedDecalsList_it);
                    //animatedDecalsList_it--;
                    //continue;
                }
            }
        }

        AnimationPtr animatedDecal = std::make_shared<tibia::Animation>(tileX, tileY, z, animationId[0], animationId[1]);
        animatedDecal->setFrameTime(frameTime);

        m_animatedDecalsSpawnList.push_back(animatedDecal);
    }

    void spawnProjectile(tibia::Creature* creature, int projectileType, int direction, sf::Vector2f origin, sf::Vector2f destination, bool isPrecise = false, bool isChild = false)
    {
        ProjectilePtr projectile = std::make_shared<tibia::Projectile>(projectileType, direction, origin, destination, isPrecise, isChild);
        projectile->setTileCoords(origin.x, origin.y);
        projectile->setZ(creature->getZ());
        projectile->setCreatureOwner(creature);

        m_projectilesSpawnList.push_back(projectile);
    }

    void drawCreatureBars()
    {
        if (m_creaturesList.size() == 0)
        {
            return;
        }

        sf::RectangleShape barBackground(sf::Vector2f(tibia::GuiData::creatureBarWidth, tibia::GuiData::creatureBarHeight));
        barBackground.setFillColor(tibia::Colors::black);
        barBackground.setOutlineThickness(1);
        barBackground.setOutlineColor(tibia::Colors::black);

        sf::RectangleShape barHealth(sf::Vector2f(tibia::GuiData::creatureBarWidth, tibia::GuiData::creatureBarHeight));
        barHealth.setFillColor(tibia::Colors::white);
        barHealth.setOutlineThickness(0);

        for (auto creature : m_creaturesList)
        {
            if (creature->isPlayer() == true)
            {
                continue;
            }

            if (creature->isDead() == true)// || creature->getHp() == creature->getHpMax())
            {
                continue;
            }

            if (creature->getZ() != m_player->getZ())
            {
                continue;
            }

            if (creature->getDistanceFromPlayer() > tibia::DRAW_DISTANCE_MAX)
            {
                continue;
            }

            switch (creature->getTeam())
            {
                case tibia::Teams::neutral:
                    continue;
                    break;

                case tibia::Teams::good:
                    barHealth.setFillColor(tibia::Colors::green);
                    break;

                case tibia::Teams::evil:
                    barHealth.setFillColor(tibia::Colors::red);
                    break;
            }

            sf::Vector2f barPosition
            (
                creature->getTileX() + ((tibia::TILE_SIZE - tibia::GuiData::creatureBarWidth) / 2) - tibia::TILE_DRAW_OFFSET,
                creature->getTileY() - (tibia::TILE_SIZE / 2)
            );

            if (creature->isSitting() == true)
            {
                barPosition.x -= tibia::TILE_DRAW_OFFSET;
                barPosition.y -= tibia::TILE_DRAW_OFFSET;
            }

            barBackground.setPosition(barPosition);
            barHealth.setPosition(barPosition);

            int hp    = creature->getHp();
            int hpMax = creature->getHpMax();

            //int hpPercent = hpMax / hp;

            barHealth.setSize
            (
                sf::Vector2f
                (
                    hp * tibia::GuiData::creatureBarWidth / hpMax,
                    barHealth.getSize().y
                )
            );

            m_window.draw(barBackground);
            m_window.draw(barHealth);
        }
    }

    void drawThings()
    {
        for (auto thingsSpawnList_it = m_thingsSpawnList.begin(); thingsSpawnList_it != m_thingsSpawnList.end(); thingsSpawnList_it++)
        {
            m_thingsList.push_back(*thingsSpawnList_it);
        }
        m_thingsSpawnList.clear();

        if (m_thingsList.size() == 0)
        {
            return;
        }

        std::stable_sort(m_thingsList.begin(), m_thingsList.end(), tibia::Thing::sortByTileCoords());

        for (auto thing : m_thingsList)
        {
            if (thing == nullptr)
            {
                return;
            }

            m_window.draw(*thing);
        }

        m_thingsList.clear();
    }

    void drawPlayer()
    {
        m_window.draw(*m_player);
    }

    void drawCreatures(bool deadOnly = false)
    {
        for (auto creaturesSpawnList_it = m_creaturesSpawnList.begin(); creaturesSpawnList_it != m_creaturesSpawnList.end(); creaturesSpawnList_it++)
        {
            m_creaturesList.push_back(*creaturesSpawnList_it);
        }
        m_creaturesSpawnList.clear();

        if (m_creaturesList.size() == 0)
        {
            return;
        }

        for (auto creaturesList_it = m_creaturesList.begin(); creaturesList_it != m_creaturesList.end(); creaturesList_it++)
        {
            tibia::Creature* creature = creaturesList_it->get();

            if (creature == nullptr)
            {
                return;
            }

            if (creature->hasDecayed() == true)
            {
                creaturesList_it = m_creaturesList.erase(creaturesList_it);
                creaturesList_it--;
                continue;
            }

            if (deadOnly == true)
            {
                if (creature->isDead() == false)
                {
                    continue;
                }
            }
            else
            {
                if (creature->isDead() == true)
                {
                    continue;
                }
            }

            if (m_player->getZ() != tibia::ZAxis::underGround && creature->getZ() == tibia::ZAxis::underGround)
            {
                continue;
            }

            if (m_player->getZ() == tibia::ZAxis::underGround && creature->getZ() != tibia::ZAxis::underGround)
            {
                continue;
            }

            if (creature->getDistanceFromPlayer() > tibia::DRAW_DISTANCE_MAX)
            {
                continue;
            }

            //std::cout << "Drawing creature: " << creature->getName() << std::endl;

            //if (creature->isDead() == true)
            //{
                //m_window.draw(*creature);
            //}
            //else
            //{
                m_thingsSpawnList.push_back(creature);
            //}
        }
    }

    void drawObjects()
    {
        for (auto objectsSpawnList_it = m_objectsSpawnList.begin(); objectsSpawnList_it != m_objectsSpawnList.end(); objectsSpawnList_it++)
        {
            m_objectsList.push_back(*objectsSpawnList_it);
        }
        m_objectsSpawnList.clear();

        if (m_objectsList.size() == 0)
        {
            return;
        }

        for (auto objectsList_it = m_objectsList.begin(); objectsList_it != m_objectsList.end(); objectsList_it++)
        {
            tibia::Object* object = objectsList_it->get();

            if (object == nullptr)
            {
                return;
            }

            if (m_player->getZ() != tibia::ZAxis::underGround && object->getZ() == tibia::ZAxis::underGround)
            {
                continue;
            }

            if (m_player->getZ() == tibia::ZAxis::underGround && object->getZ() != tibia::ZAxis::underGround)
            {
                continue;
            }

            if (calculateDistanceBetweenThings(m_player.get(), object) > tibia::DRAW_DISTANCE_MAX)
            {
                continue;
            }

            //if (object->getTileX() > m_player->getTileX() && object->getTileY() > m_player->getTileY())
            //{
                //continue;
            //}

            // fix object drawing hack
            for (auto spriteId : tibia::SpriteData::fixDrawObjects)
            {
                if (object->getId() == spriteId)
                {
                    object->setTileX(object->getTileX() + tibia::TILE_SIZE);
                    break;
                }
            }

            m_thingsSpawnList.push_back(object);
        }
    }

    void drawAnimations()
    {
        for (auto animationsSpawnList_it = m_animationsSpawnList.begin(); animationsSpawnList_it != m_animationsSpawnList.end(); animationsSpawnList_it++)
        {
            m_animationsList.push_back(*animationsSpawnList_it);
        }
        m_animationsSpawnList.clear();

        if (m_animationsList.size() == 0)
        {
            return;
        }

        for (auto animationsList_it = m_animationsList.begin(); animationsList_it != m_animationsList.end(); animationsList_it++)
        {
            tibia::Animation* animation = animationsList_it->get();

            if (animation == nullptr)
            {
                return;
            }

            if (animation->getCurrentFrame() > animation->getNumFrames() - 1)
            {
                animationsList_it = m_animationsList.erase(animationsList_it);
                animationsList_it--;
                continue;
            }

            if (m_player->getZ() != tibia::ZAxis::underGround && animation->getZ() == tibia::ZAxis::underGround)
            {
                continue;
            }

            if (m_player->getZ() == tibia::ZAxis::underGround && animation->getZ() != tibia::ZAxis::underGround)
            {
                continue;
            }

            m_thingsSpawnList.push_back(animation);
        }
    }

    void drawAnimatedDecals()
    {
        for (auto animatedDecalsSpawnList_it = m_animatedDecalsSpawnList.begin(); animatedDecalsSpawnList_it != m_animatedDecalsSpawnList.end(); animatedDecalsSpawnList_it++)
        {
            m_animatedDecalsList.push_back(*animatedDecalsSpawnList_it);
        }
        m_animatedDecalsSpawnList.clear();

        if (m_animatedDecalsList.size() == 0)
        {
            return;
        }

        for (auto animatedDecalsList_it = m_animatedDecalsList.begin(); animatedDecalsList_it != m_animatedDecalsList.end(); animatedDecalsList_it++)
        {
            tibia::Animation* animatedDecal = animatedDecalsList_it->get();

            if (animatedDecal == nullptr)
            {
                return;
            }

            if (animatedDecal->getCurrentFrame() > animatedDecal->getNumFrames() - 1)
            {
                animatedDecalsList_it = m_animatedDecalsList.erase(animatedDecalsList_it);
                animatedDecalsList_it--;
                continue;
            }

            if (m_player->getZ() != tibia::ZAxis::underGround && animatedDecal->getZ() == tibia::ZAxis::underGround)
            {
                continue;
            }

            if (m_player->getZ() == tibia::ZAxis::underGround && animatedDecal->getZ() != tibia::ZAxis::underGround)
            {
                continue;
            }

            if (calculateDistanceBetweenThings(m_player.get(), animatedDecal) > tibia::DRAW_DISTANCE_MAX)
            {
                continue;
            }

            //m_window.draw(*animatedDecal);
            m_thingsSpawnList.push_back(animatedDecal);
        }
    }

    void drawProjectiles()
    {
        for (auto projectilesSpawnList_it = m_projectilesSpawnList.begin(); projectilesSpawnList_it != m_projectilesSpawnList.end(); projectilesSpawnList_it++)
        {
            m_projectilesList.push_back(*projectilesSpawnList_it);
        }
        m_projectilesSpawnList.clear();

        if (m_projectilesList.size() == 0)
        {
            return;
        }

        for (auto projectilesList_it = m_projectilesList.begin(); projectilesList_it != m_projectilesList.end(); projectilesList_it++)
        {
            tibia::Projectile* projectile = projectilesList_it->get();

            if (projectile == nullptr)
            {
                return;
            }

            if (m_player->getZ() != tibia::ZAxis::underGround && projectile->getZ() == tibia::ZAxis::underGround)
            {
                continue;
            }

            if (m_player->getZ() == tibia::ZAxis::underGround && projectile->getZ() != tibia::ZAxis::underGround)
            {
                continue;
            }

            sf::Vector2f projectileSpritePosition     = projectile->getSpritePosition();
            sf::Vector2f projectileSpriteTilePosition = projectile->getSpriteTilePosition();

            //std::cout << "projectileSpriteTilePosition x,y: " << projectileSpriteTilePosition.x << "," << projectileSpriteTilePosition.y << std::endl;

            if
            (
                projectileSpriteTilePosition.x < 0 ||
                projectileSpriteTilePosition.y < 0 ||
                projectileSpriteTilePosition.x > tibia::MAP_TILE_XY_MAX ||
                projectileSpriteTilePosition.y > tibia::MAP_TILE_XY_MAX
            )
            {
                projectilesList_it = m_projectilesList.erase(projectilesList_it);
                projectilesList_it--;
                continue;
            }

            int projectileDistanceTravelled = projectile->getDistanceTravelled();

            if (projectileDistanceTravelled > 0 && projectileDistanceTravelled % tibia::TILE_SIZE == 0)
            {
                if (checkTileIsBlockProjectiles(sf::Vector2u(projectileSpriteTilePosition.x, projectileSpriteTilePosition.y), projectile->getZ()) == true)
                {
                    spawnAnimation
                    (
                        projectileSpriteTilePosition.x,
                        projectileSpriteTilePosition.y,
                        projectile->getZ(),
                        projectile->getAnimationOnBlock()
                    );

                    projectilesList_it = m_projectilesList.erase(projectilesList_it);
                    projectilesList_it--;
                    continue;
                }

                tibia::Creature* attacker = projectile->getCreatureOwner();
                tibia::Creature* defender = checkTileHasCreature(sf::Vector2u(projectileSpriteTilePosition.x, projectileSpriteTilePosition.y), projectile->getZ());

                if (attacker != nullptr && defender != nullptr)
                {
                    int* projectileAnimatedDecalOnKill = tibia::AnimatedDecals::poolRed;

                    if (projectile->getType() == tibia::ProjectileTypes::arrowPoison)
                    {
                        projectileAnimatedDecalOnKill = tibia::AnimatedDecals::poolGreen;
                    }

                    bool creatureIsDamaged = handleCreatureDamage
                    (
                        attacker,
                        defender,
                        projectile->getDamage(),
                        projectile->getAnimationOnHit(),
                        projectile->getAnimatedDecalOnHit(),
                        projectileAnimatedDecalOnKill
                    );

                    if (creatureIsDamaged == true)
                    {
                        projectilesList_it = m_projectilesList.erase(projectilesList_it);
                        projectilesList_it--;
                        continue;
                    }
                }

                int projectileTileDistanceTravelled = projectile->getTileDistanceTravelled();

                //std::cout << "projectileTileDistanceTravelled: " << projectileTileDistanceTravelled << std::endl;

                if
                (
                    projectileTileDistanceTravelled >= projectile->getRange()
                    //std::abs(projectileSpritePosition.x) >= (projectile->getRange() * tibia::TILE_SIZE) ||
                    //std::abs(projectileSpritePosition.y) >= (projectile->getRange() * tibia::TILE_SIZE)
                )
                {
                    int *projectileAnimationOnMiss = tibia::Animations::hitMiss;

                    if (checkTileIsWater(sf::Vector2u(projectileSpriteTilePosition.x, projectileSpriteTilePosition.y), projectile->getZ()) == true)
                    {
                        projectileAnimationOnMiss = tibia::Animations::waterSplash;
                    }

                    spawnAnimation
                    (
                        projectileSpriteTilePosition.x,
                        projectileSpriteTilePosition.y,
                        projectile->getZ(),
                        projectileAnimationOnMiss
                    );

                    projectilesList_it = m_projectilesList.erase(projectilesList_it);
                    projectilesList_it--;
                    continue;
                }
            }

            //tibia::Sprite spr;
            //spr.setId(1);
            //spr.setPosition(projectile->getSpriteTilePosition().x, projectile->getSpriteTilePosition().y);
            //m_window.draw(spr);

            m_thingsSpawnList.push_back(projectile);
        }
    }

    void doAnimatedWater()
    {
        if (isPlayerNearWater() == false)
        {
            return;
        }

        m_map.tileMapGroundTiles.doAnimatedWater();
    }

    void doAnimatedObjects()
    {
        for (auto object : m_objectsList)
        {
            if (object->isAnimated() == false)
            {
                continue;
            }

            if (m_player->getZ() != tibia::ZAxis::underGround && object->getZ() == tibia::ZAxis::underGround)
            {
                continue;
            }

            if (m_player->getZ() == tibia::ZAxis::underGround && object->getZ() != tibia::ZAxis::underGround)
            {
                continue;
            }

            if (calculateDistanceBetweenThings(m_player.get(), object.get()) > tibia::DRAW_DISTANCE_MAX)
            {
                continue;
            }

            bool animationIsDone = false;

            int objectId = object->getId();

            for (unsigned int j = 0; j < tibia::animatedObjectsList.size(); j++)
            {
                if (animationIsDone == true)
                {
                    break;
                }

                std::vector<int> sprites = tibia::animatedObjectsList.at(j);

                for (unsigned int n = 0; n < sprites.size(); n++)
                {
                    int spriteId = sprites.at(n);

                    if (objectId == spriteId)
                    {
                        n++;

                        if (n > sprites.size() - 1)
                        {
                            n = 0;
                        }

                        spriteId = sprites.at(n);

                        object->setId(spriteId);

                        animationIsDone = true;

                        break;
                    }
                }
            }
        }
    }

    void drawTextList(std::vector<sf::Text> textList)
    {
        int textPositionOffsetY = 0;

        for (auto text : textList)
        {
            //std::string textString = text.getString();
            //std::cout << textString << std::endl;

            text.setPosition(text.getPosition().x, text.getPosition().y + textPositionOffsetY);

            sf::Text textShadow = text;

            textShadow.setColor(tibia::Colors::gameTextShadowColor);

            textShadow.setPosition(text.getPosition().x + 1, text.getPosition().y + 1);
            m_window.draw(textShadow);

            textShadow.setPosition(text.getPosition().x - 1, text.getPosition().y - 1);
            m_window.draw(textShadow);

            textShadow.setPosition(text.getPosition().x + 1, text.getPosition().y);
            m_window.draw(textShadow);

            textShadow.setPosition(text.getPosition().x - 1, text.getPosition().y);
            m_window.draw(textShadow);

            textShadow.setPosition(text.getPosition().x,     text.getPosition().y + 1);
            m_window.draw(textShadow);

            textShadow.setPosition(text.getPosition().x,     text.getPosition().y - 1);
            m_window.draw(textShadow);

            textShadow.setPosition(text.getPosition().x + 1, text.getPosition().y - 1);
            m_window.draw(textShadow);

            textShadow.setPosition(text.getPosition().x - 1, text.getPosition().y + 1);
            m_window.draw(textShadow);

            m_window.draw(text);

            textPositionOffsetY += tibia::TILE_SIZE;
        }
    }

    std::vector<sf::Text> createTextList(std::string text, unsigned int textSize, sf::Color textColor, sf::Vector2f textPosition)
    {
        std::vector<sf::Text> textList;

        std::vector<std::string> textTokens;

        boost::split(textTokens, text, boost::is_any_of("\n"));

        for (auto textToken : textTokens)
        {
            sf::Text text;
            text.setString(textToken);
            text.setFont(m_font);
            text.setCharacterSize(textSize);
            text.setColor(textColor);

            //sf::FloatRect textRect = text.getLocalBounds();

            //text.setOrigin
            //(
                //textRect.left + (textRect.width  / 2),
                //textRect.top  + (textRect.height / 2)
            //);

            text.setPosition
            (
                textPosition.x - (text.getLocalBounds().width / 2),
                textPosition.y
            );

            textList.push_back(text);
        }

        return textList;
    }

    void showGameText(std::string text, unsigned int textSize, sf::Color textColor)
    {
        int textOffsetY = 1;

        std::size_t findNewLine = text.find("\n");

        if (findNewLine != std::string::npos)
        {
            textOffsetY++;
        }

        m_textList.clear();

        m_textList = createTextList
        (
            text,
            textSize,
            textColor,
            sf::Vector2f
            (
                (m_player->getTileX() + tibia::TILE_DRAW_OFFSET),
                (m_player->getTileY() - (tibia::TILE_SIZE * textOffsetY))
            )
        );

        m_clockText.restart();
    }

    void drawGameText()
    {
        sf::Time timeText = m_clockText.getElapsedTime();

        if (timeText.asSeconds() < tibia::TEXT_TIME)
        {
            drawTextList(m_textList);
        }
    }

    void drawTileMap(tibia::TileMap* tileMap)
    {
        int x = ((m_windowView.getCenter().x - (tibia::TILE_SIZE / 2)) / tibia::TILE_SIZE) - NUM_TILES_FROM_CENTER_X;
        int y = ((m_windowView.getCenter().y - (tibia::TILE_SIZE / 2)) / tibia::TILE_SIZE) - NUM_TILES_FROM_CENTER_Y;

        if (x < 0) x = 0;
        if (y < 0) y = 0;

        std::vector<int>* tiles = tileMap->getTiles();

        if (tiles->size() == 0)
        {
            return;
        }

        std::vector<tibia::TileMap::TilePtr> *tilesList = tileMap->getTilesList();

        m_tileVertices.clear();

        for (unsigned int i = x; i < x + tibia::NUM_TILES_X; i++)
        {
            for (unsigned int j = y; j < y + tibia::NUM_TILES_Y; j++)
            {
                if (i > tibia::MAP_SIZE - 1) continue;
                if (j > tibia::MAP_SIZE - 1) continue;

                int tileNumber = i + j * tibia::MAP_SIZE;

                if (tileNumber < 0 || tileNumber > tibia::TILE_NUMBER_MAX)
                {
                    continue;
                }

                tibia::Tile* tile = tilesList->at(tileNumber).get();

                int tileId = tile->getId(); //tiles->at(tileNumber);

                if (tileId == tibia::TILE_NULL || tileId == 1)
                {
                    continue;
                }

                int tu = (tileId - 1) % (tibia::Textures::sprites.getSize().x / tibia::TILE_SIZE);
                int tv = (tileId - 1) / (tibia::Textures::sprites.getSize().x / tibia::TILE_SIZE);

                sf::Vertex quad[4];

                quad[0].position = sf::Vector2f(i       * tibia::TILE_SIZE, j       * tibia::TILE_SIZE);
                quad[1].position = sf::Vector2f((i + 1) * tibia::TILE_SIZE, j       * tibia::TILE_SIZE);
                quad[2].position = sf::Vector2f((i + 1) * tibia::TILE_SIZE, (j + 1) * tibia::TILE_SIZE);
                quad[3].position = sf::Vector2f(i       * tibia::TILE_SIZE, (j + 1) * tibia::TILE_SIZE);

                quad[0].texCoords = sf::Vector2f(tu       * tibia::TILE_SIZE, tv       * tibia::TILE_SIZE);
                quad[1].texCoords = sf::Vector2f((tu + 1) * tibia::TILE_SIZE, tv       * tibia::TILE_SIZE);
                quad[2].texCoords = sf::Vector2f((tu + 1) * tibia::TILE_SIZE, (tv + 1) * tibia::TILE_SIZE);
                quad[3].texCoords = sf::Vector2f(tu       * tibia::TILE_SIZE, (tv + 1) * tibia::TILE_SIZE);

                if (tile->getFlags() & tibia::TileFlags::offset)
                {
                    quad[0].position.x -= tibia::TILE_DRAW_OFFSET;
                    quad[1].position.x -= tibia::TILE_DRAW_OFFSET;
                    quad[2].position.x -= tibia::TILE_DRAW_OFFSET;
                    quad[3].position.x -= tibia::TILE_DRAW_OFFSET;

                    quad[0].position.y -= tibia::TILE_DRAW_OFFSET;
                    quad[1].position.y -= tibia::TILE_DRAW_OFFSET;
                    quad[2].position.y -= tibia::TILE_DRAW_OFFSET;
                    quad[3].position.y -= tibia::TILE_DRAW_OFFSET;
                }

                m_tileVertices.append(quad[0]);
                m_tileVertices.append(quad[1]);
                m_tileVertices.append(quad[2]);
                m_tileVertices.append(quad[3]);
            }
        }

        sf::RenderStates states;
        states.texture = &tibia::Textures::sprites;

        m_window.draw(m_tileVertices, states);
    }

    void drawGameWindow(sf::RenderWindow* mainWindow)
    {
        m_windowView.setCenter
        (
            m_player->getTileX() + (tibia::TILE_SIZE / 2),
            m_player->getTileY() + (tibia::TILE_SIZE / 2)
        );

        m_window.setView(m_windowView);
        m_window.clear(tibia::Colors::black);

        int playerX = m_player->getTileX();
        int playerY = m_player->getTileY();
        int playerZ = m_player->getZ();

        if (playerZ == tibia::ZAxis::underGround)
        {
            drawTileMap(&m_map.tileMapUnderGroundTiles);
            drawTileMap(&m_map.tileMapUnderGroundEdges);
            drawTileMap(&m_map.tileMapUnderGroundObjects);
        }
        else
        {
            drawTileMap(&m_map.tileMapGroundTiles);
            drawTileMap(&m_map.tileMapGroundEdges);
            drawTileMap(&m_map.tileMapGroundObjects);
        }

        //////////////////////////////////////////////////

        drawAnimatedDecals();

        drawCreatures(true);

        drawObjects();

        drawCreatures(false);

        drawProjectiles();

        drawAnimations();

        drawThings(); // draw sorted by y-axis

        //////////////////////////////////////////////////

/*
        for (auto creature : m_creaturesList)
        {
            if (creature->isPlayer() == true || creature->isDead() == true)
            {
                continue;
            }

            if (creature->getZ() != m_player->getZ())
            {
                continue;
            }

            if (creature->getDistanceFromPlayer() > tibia::DRAW_DISTANCE_MAX)
            {
                continue;
            }

            sf::Text text(creature->getName(), m_fontSmall, tibia::FontSizes::small);

            int textX = creature->getTileX() - (text.getLocalBounds().width / 2) + (tibia::TILE_SIZE / 2) - tibia::TILE_DRAW_OFFSET;
            int textY = creature->getTileY() - tibia::TILE_SIZE - (tibia::TILE_DRAW_OFFSET / 2) - 1;

            if (creature->isSitting() == true)
            {
                textX -= tibia::TILE_DRAW_OFFSET;
                textY -= tibia::TILE_DRAW_OFFSET;
            }

            text.setColor(tibia::Colors::black);
            text.setPosition
            (
                textX + 1,
                textY + 1
            );

            m_window.draw(text);

            sf::Color creatureColor = tibia::Colors::white;

            switch (creature->getTeam())
            {
                case tibia::Teams::good:
                    creatureColor = tibia::Colors::green;
                    break;

                case tibia::Teams::evil:
                    creatureColor = tibia::Colors::red;
                    break;
            }

            text.setColor(creatureColor);
            text.setPosition
            (
                textX,
                textY
            );

            m_window.draw(text);
        }
*/

/*
        for (auto object : m_objectsList)
        {
            std::cout << object->getTileNumber() << std::endl;

            std::stringstream objectTextDisplayed;
            objectTextDisplayed << object->getTileNumber();

            sf::Text objectText(objectTextDisplayed.str(), m_fontSmall, tibia::FontSizes::small);

            objectText.setColor(tibia::Colors::black);
            objectText.setPosition
            (
                object->getTileX() + 1,
                object->getTileY() + 1
            );

            m_window.draw(objectText);

            objectText.setColor(tibia::Colors::pink);
            objectText.setPosition
            (
                object->getTileX(),
                object->getTileY()
            );

            m_window.draw(objectText);
        }
*/

        if (playerZ != tibia::ZAxis::underGround)
        {
            bool drawAboveGround = true;

            if (playerZ == tibia::ZAxis::ground)
            {
                std::vector<int>* aboveGroundTiles = m_map.tileMapAboveGroundTiles.getTiles();

                std::vector<int> aboveGroundTileNumbers;

                for (int i = -2; i < 3; i++)
                {
                    aboveGroundTileNumbers.push_back(tibia::getTileNumberByTileCoords(playerX - (2 * tibia::TILE_SIZE), playerY + (i * tibia::TILE_SIZE)));
                    aboveGroundTileNumbers.push_back(tibia::getTileNumberByTileCoords(playerX - (1 * tibia::TILE_SIZE), playerY + (i * tibia::TILE_SIZE)));
                    aboveGroundTileNumbers.push_back(tibia::getTileNumberByTileCoords(playerX                         , playerY + (i * tibia::TILE_SIZE)));
                    aboveGroundTileNumbers.push_back(tibia::getTileNumberByTileCoords(playerX + (1 * tibia::TILE_SIZE), playerY + (i * tibia::TILE_SIZE)));
                    aboveGroundTileNumbers.push_back(tibia::getTileNumberByTileCoords(playerX + (2 * tibia::TILE_SIZE), playerY + (i * tibia::TILE_SIZE)));
                }

                for (auto tileNumber : aboveGroundTileNumbers)
                {
                    if (tileNumber < 0 || tileNumber > tibia::TILE_NUMBER_MAX)
                    {
                        continue;
                    }

                    if (aboveGroundTiles->at(tileNumber) != tibia::TILE_NULL)
                    {
                        drawAboveGround = false;
                        break;
                    }
                }
            }

            if (drawAboveGround == true)
            {
                drawTileMap(&m_map.tileMapAboveGroundTiles);
                drawTileMap(&m_map.tileMapAboveGroundEdges);
                drawTileMap(&m_map.tileMapAboveGroundObjects);

                if (playerZ == tibia::ZAxis::aboveGround)
                {
                    //////////////////////////////////////////////////

                    drawPlayer();

                    //////////////////////////////////////////////////
                }
            }
        }

        if (playerZ == tibia::ZAxis::underGround)
        {
            m_rtLight.clear(tibia::Colors::black);

            for (auto creature : m_creaturesList)
            {
                if (creature->getZ() != tibia::ZAxis::underGround)
                {
                    continue;
                }

                if (creature->getDistanceFromPlayer() > tibia::DRAW_DISTANCE_MAX)
                {
                    continue;
                }

                sf::Sprite spriteLight;

                if (creature->isPlayer() == true)
                {
                    spriteLight.setTexture(tibia::Textures::light3);
                }
                else
                {
                    spriteLight.setTexture(tibia::Textures::light2);
                }

                spriteLight.setOrigin(spriteLight.getLocalBounds().width / 2, spriteLight.getLocalBounds().height / 2);
                spriteLight.setPosition(creature->getTileX() + (tibia::TILE_SIZE / 2), creature->getTileY() + (tibia::TILE_SIZE / 2));

                spriteLight.setColor(tibia::Colors::light);

                m_rtLight.draw(spriteLight, sf::BlendMode::BlendAdd);
            }

            for (auto object : m_objectsList)
            {
                if (object->getZ() != tibia::ZAxis::underGround)
                {
                    continue;
                }

                if (calculateDistanceBetweenThings(m_player.get(), object.get()) > tibia::DRAW_DISTANCE_MAX)
                {
                    continue;
                }

                if (checkTileIsLight(object->getTilePosition(), tibia::ZAxis::underGround) == false)
                {
                    continue;
                }

                sf::Sprite spriteLight;

                if (object->getId() == tibia::SpriteData::ladder || object->getId() == tibia::SpriteData::stairs)
                {
                    spriteLight.setTexture(tibia::Textures::light);
                }
                else
                {
                    spriteLight.setTexture(tibia::Textures::light2);
                }

                spriteLight.setOrigin(spriteLight.getLocalBounds().width / 2, spriteLight.getLocalBounds().height / 2);
                spriteLight.setPosition(object->getTileX() + (tibia::TILE_SIZE / 2), object->getTileY() + (tibia::TILE_SIZE / 2));

                spriteLight.setColor(tibia::Colors::light);

                m_rtLight.draw(spriteLight, sf::BlendMode::BlendAdd);
            }

            auto underGroundObjects = m_map.tileMapUnderGroundObjects.getTilesList();

            for (auto tile : *underGroundObjects)
            {
                int tileNumber = tile->getNumber();

                int tileId = tile->getId();

                if (tileId == tibia::TILE_NULL)
                {
                    continue;
                }

                sf::Vector2u tileCoords = tibia::getTileCoordsByTileNumber(tileNumber);

                //std::cout << "tileCoords: " << tileCoords.x << "," << tileCoords.y << std::endl;

                //tibia::Sprite spr;
                //spr.setId(1);
                //spr.setPosition(tileCoords.x, tileCoords.y);
                //m_window.draw(spr);

                if (calculateDistanceByTile(m_player->getTileX(), m_player->getTileY(), tileCoords.x, tileCoords.y) > tibia::DRAW_DISTANCE_MAX)
                {
                    continue;
                }

                if (checkTileIsLight(tileCoords, tibia::ZAxis::underGround) == false)
                {
                    continue;
                }

                sf::Sprite spriteLight;

                spriteLight.setTexture(tibia::Textures::light2);

                spriteLight.setOrigin(spriteLight.getLocalBounds().width / 2, spriteLight.getLocalBounds().height / 2);
                spriteLight.setPosition(tileCoords.x + (tibia::TILE_SIZE / 2), tileCoords.y + (tibia::TILE_SIZE / 2));

                spriteLight.setColor(tibia::Colors::light);

                m_rtLight.draw(spriteLight, sf::BlendMode::BlendAdd);
            }

            for (auto projectile : m_projectilesList)
            {
                if (projectile->getZ() != tibia::ZAxis::underGround)
                {
                    continue;
                }

                sf::Vector2u tileCoords = static_cast<sf::Vector2u>(projectile->getSpriteTilePosition());

                sf::Sprite spriteLight;

                spriteLight.setTexture(tibia::Textures::light);

                spriteLight.setOrigin(spriteLight.getLocalBounds().width / 2, spriteLight.getLocalBounds().height / 2);
                spriteLight.setPosition(tileCoords.x + (tibia::TILE_SIZE / 2), tileCoords.y + (tibia::TILE_SIZE / 2));

                spriteLight.setColor(tibia::Colors::light);

                m_rtLight.draw(spriteLight, sf::BlendMode::BlendAdd);
            }

            for (auto animation : m_animationsList)
            {
                if (animation->getZ() != tibia::ZAxis::underGround)
                {
                    continue;
                }

                sf::Vector2u tileCoords = animation->getTilePosition();

                sf::Sprite spriteLight;

                spriteLight.setTexture(tibia::Textures::light);

                spriteLight.setOrigin(spriteLight.getLocalBounds().width / 2, spriteLight.getLocalBounds().height / 2);
                spriteLight.setPosition(tileCoords.x + (tibia::TILE_SIZE / 2), tileCoords.y + (tibia::TILE_SIZE / 2));

                spriteLight.setColor(tibia::Colors::light);

                m_rtLight.draw(spriteLight, sf::BlendMode::BlendAdd);
            }

            m_rtLight.display();

            m_rectLight.setTexture(&m_rtLight.getTexture());

            m_window.draw(m_rectLight, sf::BlendMode::BlendMultiply);
        }

        drawCreatureBars();

        drawGameText();

        m_window.display();

        m_windowSprite.setTexture(m_window.getTexture());
        m_windowSprite.setPosition(tibia::GuiData::gameWindowX, tibia::GuiData::gameWindowY);

        mainWindow->draw(m_windowBorder);
        mainWindow->draw(m_windowSprite);
    }

    void addMiniMapObjects(std::vector<sf::Vertex> &verticesList)
    {
        for (auto object : m_objectsList)
        {
            if (object->getZ() != m_player->getZ())
            {
                continue;
            }

            if (calculateDistanceBetweenThings(m_player.get(), object.get()) > (tibia::DRAW_DISTANCE_MAX * 2))
            {
                continue;
            }

            int tileFlags = tibia::spriteFlags[object->getId()];

            if (tileFlags & (tibia::TileFlags::solid | tibia::TileFlags::ladder | tibia::TileFlags::moveAbove | tibia::TileFlags::moveBelow))
            {
                sf::Color objectColor;

                if (tileFlags & tibia::TileFlags::solid)
                {
                    objectColor = tibia::Colors::tileIsSolid;
                }

                if (tileFlags & (tibia::TileFlags::ladder | tibia::TileFlags::moveAbove | tibia::TileFlags::moveBelow))
                {
                    objectColor = tibia::Colors::tileIsMoveAboveOrBelow;
                }

                sf::Vertex quad[4];

                quad[0].position = sf::Vector2f(object->getTileX(),                    object->getTileY());
                quad[1].position = sf::Vector2f(object->getTileX() + tibia::TILE_SIZE, object->getTileY());
                quad[2].position = sf::Vector2f(object->getTileX() + tibia::TILE_SIZE, object->getTileY() + tibia::TILE_SIZE);
                quad[3].position = sf::Vector2f(object->getTileX(),                    object->getTileY() + tibia::TILE_SIZE);

                quad[0].color = objectColor;
                quad[1].color = objectColor;
                quad[2].color = objectColor;
                quad[3].color = objectColor;

                verticesList.push_back(quad[0]);
                verticesList.push_back(quad[1]);
                verticesList.push_back(quad[2]);
                verticesList.push_back(quad[3]);
            }
        }
    }

    void addMiniMapCreatures(std::vector<sf::Vertex> &verticesList)
    {
        for (auto creature : m_creaturesList)
        {
            if (creature->isPlayer() == true)
            {
                continue;
            }

            if (creature->isDead() == true)
            {
                continue;
            }

            if (creature->getZ() != m_player->getZ())
            {
                continue;
            }

            if (creature->getDistanceFromPlayer() > (tibia::DRAW_DISTANCE_MAX * 2))
            {
                continue;
            }

            //int tileNumber = creature->getTileNumber();

            //if (tileNumber < 0 || tileNumber > tibia::TILE_NUMBER_MAX)
            //{
                //continue;
            //}

            sf::Vertex quad[4];

            quad[0].position = sf::Vector2f(creature->getTileX(),                    creature->getTileY());
            quad[1].position = sf::Vector2f(creature->getTileX() + tibia::TILE_SIZE, creature->getTileY());
            quad[2].position = sf::Vector2f(creature->getTileX() + tibia::TILE_SIZE, creature->getTileY() + tibia::TILE_SIZE);
            quad[3].position = sf::Vector2f(creature->getTileX(),                    creature->getTileY() + tibia::TILE_SIZE);

            //quad[0].color = tibia::Colors::transparent;
            //quad[1].color = tibia::Colors::transparent;
            //quad[2].color = tibia::Colors::transparent;
            //quad[3].color = tibia::Colors::transparent;

            sf::Color creatureColor = tibia::Colors::white;

            switch (creature->getTeam())
            {
                case tibia::Teams::good:
                    creatureColor = tibia::Colors::green;
                    break;

                case tibia::Teams::evil:
                    creatureColor = tibia::Colors::red;
                    break;
            }

            quad[0].color = creatureColor;
            quad[1].color = creatureColor;
            quad[2].color = creatureColor;
            quad[3].color = creatureColor;

            verticesList.push_back(quad[0]);
            verticesList.push_back(quad[1]);
            verticesList.push_back(quad[2]);
            verticesList.push_back(quad[3]);
        }
    }

    void updateMiniMapWindow()
    {
        m_miniMapWindow.clear(tibia::Colors::black);

        std::vector<sf::Vertex> miniMapVertices;

        switch (m_player->getZ())
        {
            case tibia::ZAxis::underGround:
                m_map.tileMapUnderGroundTiles.addMiniMapTiles(miniMapVertices);
                m_map.tileMapUnderGroundObjects.addMiniMapTiles(miniMapVertices);
                break;

            case tibia::ZAxis::ground:
                m_map.tileMapGroundTiles.addMiniMapTiles(miniMapVertices);
                m_map.tileMapGroundObjects.addMiniMapTiles(miniMapVertices);
                break;

            case tibia::ZAxis::aboveGround:
                m_map.tileMapAboveGroundTiles.addMiniMapTiles(miniMapVertices);
                m_map.tileMapAboveGroundObjects.addMiniMapTiles(miniMapVertices);
                break;
        }

        addMiniMapObjects(miniMapVertices);

        addMiniMapCreatures(miniMapVertices);

        sf::Vertex quadPlayer[4];

        quadPlayer[0].position = sf::Vector2f(m_player->getTileX(),                    m_player->getTileY());
        quadPlayer[1].position = sf::Vector2f(m_player->getTileX() + tibia::TILE_SIZE, m_player->getTileY());
        quadPlayer[2].position = sf::Vector2f(m_player->getTileX() + tibia::TILE_SIZE, m_player->getTileY() + tibia::TILE_SIZE);
        quadPlayer[3].position = sf::Vector2f(m_player->getTileX(),                    m_player->getTileY() + tibia::TILE_SIZE);

        quadPlayer[0].color = tibia::Colors::pink;
        quadPlayer[1].color = tibia::Colors::pink;
        quadPlayer[2].color = tibia::Colors::pink;
        quadPlayer[3].color = tibia::Colors::pink;

        miniMapVertices.push_back(quadPlayer[0]);
        miniMapVertices.push_back(quadPlayer[1]);
        miniMapVertices.push_back(quadPlayer[2]);
        miniMapVertices.push_back(quadPlayer[3]);

        m_miniMapWindow.draw(&miniMapVertices[0], miniMapVertices.size(), sf::Quads);
    }

    void drawMiniMapWindow(sf::RenderWindow* mainWindow)
    {
        m_miniMapWindowView.setCenter
        (
            m_player->getTileX() + (tibia::TILE_SIZE / 2),
            m_player->getTileY() + (tibia::TILE_SIZE / 2)
        );

        m_miniMapWindow.setView(m_miniMapWindowView);

        m_miniMapWindow.display();

        m_miniMapWindowSprite.setTexture(m_miniMapWindow.getTexture());
        m_miniMapWindowSprite.setPosition(tibia::GuiData::miniMapWindowX, tibia::GuiData::miniMapWindowY);

        mainWindow->draw(m_miniMapWindowBorder);
        mainWindow->draw(m_miniMapWindowSprite);
    }

    bool checkCreatureIsSitting(tibia::Creature* creature)
    {
        creature->updateTileCoords();

        if (checkTileIsChair(sf::Vector2u(creature->getTileX(), creature->getTileY()), creature->getZ()) == true)
        {
            return true;
        }

        return false;
    }

    int getTileFlags(sf::Vector2u tilePosition, int tileZ, bool tilesOnly = false, bool objectsOnly = false)
    {
        int tileFlags = 0;

        for (auto tileMapList_it = m_map.tileMapList.begin(); tileMapList_it != m_map.tileMapList.end(); tileMapList_it++)
        {
            tibia::TileMap* tileMap = *tileMapList_it;

            if (tileMap->getZ() != tileZ)
            {
                continue;
            }

            if (tileMap->getType() == tibia::TileMapTypes::edges)
            {
                continue;
            }

            if (tilesOnly == true)
            {
                if (tileMap->getType() != tibia::TileMapTypes::tiles)
                {
                    continue;
                }
            }

            if (objectsOnly == true)
            {
                if (tileMap->getType() != tibia::TileMapTypes::objects)
                {
                    continue;
                }
            }

            int tileNumber = tibia::getTileNumberByTileCoords(tilePosition.x, tilePosition.y);

            if (tileNumber < 0 || tileNumber > tibia::TILE_NUMBER_MAX)
            {
                continue;
            }

            std::vector<tibia::TileMap::TilePtr> *tilesList = tileMap->getTilesList();

            if (tilesList->size() == 0 || tilesList->size() < tileNumber)
            {
                continue;
            }

            tibia::Tile* tile = tilesList->at(tileNumber).get();

            tileFlags |= tile->getFlags();
        }

        for (auto object : m_objectsList)
        {
            if (object->getZ() != tileZ)
            {
                continue;
            }

            if
            (
                object->getPosition().x == tilePosition.x &&
                object->getPosition().y == tilePosition.y
            )
            {
                int tileObjectFlags = tibia::spriteFlags[object->getId()];

                tileFlags |= tileObjectFlags;
            }
        }

        return tileFlags;
    }

    bool checkTileIsNull(sf::Vector2u tilePosition, int tileZ)
    {
        int tileFlags = getTileFlags(tilePosition, tileZ);

        return tileFlags & tibia::TileFlags::null;
    }

    bool checkTileIsSolid(sf::Vector2u tilePosition, int tileZ)
    {
        int tileFlags = getTileFlags(tilePosition, tileZ);

        return tileFlags & tibia::TileFlags::solid;
    }

    bool checkTileIsBlockProjectiles(sf::Vector2u tilePosition, int tileZ)
    {
        int tileFlags = getTileFlags(tilePosition, tileZ);

        return tileFlags & tibia::TileFlags::blockProjectiles;
    }

    bool checkTileIsWater(sf::Vector2u tilePosition, int tileZ)
    {
        int tileFlags = getTileFlags(tilePosition, tileZ, true, false);

        return tileFlags & tibia::TileFlags::water;
    }

    bool checkTileIsLava(sf::Vector2u tilePosition, int tileZ)
    {
        int tileFlags = getTileFlags(tilePosition, tileZ, true, false);

        return tileFlags & tibia::TileFlags::lava;
    }

    bool checkTileIsChair(sf::Vector2u tilePosition, int tileZ)
    {
        int tileFlags = getTileFlags(tilePosition, tileZ, false, true);

        return tileFlags & tibia::TileFlags::chair;
    }

    bool checkTileIsOffset(sf::Vector2u tilePosition, int tileZ)
    {
        int tileFlags = getTileFlags(tilePosition, tileZ);

        return tileFlags & tibia::TileFlags::offset;
    }

    bool checkTileIsLadder(sf::Vector2u tilePosition, int tileZ)
    {
        int tileFlags = getTileFlags(tilePosition, tileZ, false, true);

        return tileFlags & tibia::TileFlags::ladder;
    }

    bool checkTileIsMoveAbove(sf::Vector2u tilePosition, int tileZ)
    {
        int tileFlags = getTileFlags(tilePosition, tileZ, false, true);

        return tileFlags & tibia::TileFlags::moveAbove;
    }

    bool checkTileIsMoveBelow(sf::Vector2u tilePosition, int tileZ)
    {
        int tileFlags = getTileFlags(tilePosition, tileZ);

        return tileFlags & tibia::TileFlags::moveBelow;
    }

    bool checkTileIsLight(sf::Vector2u tilePosition, int tileZ)
    {
        int tileFlags = getTileFlags(tilePosition, tileZ);

        return tileFlags & tibia::TileFlags::light;
    }

    tibia::Creature* checkTileHasCreature(sf::Vector2u tilePosition, int tileZ, bool skipDead = true)
    {
        tibia::Creature* foundCreature = nullptr;

        for (auto creature : m_creaturesList)
        {
            //if (creature->isPlayer() == true)
            //{
                //continue;
            //}

            if (skipDead == true)
            {
                if (creature->isDead() == true)
                {
                    continue;
                }
            }

            if (creature->getZ() == tileZ)
            {
                if (creature->getTileX() == tilePosition.x && creature->getTileY() == tilePosition.y)
                {
                    return creature.get();
                }
            }
        }

        return foundCreature;
    }

    sf::Vector2u getTileCoordsByCreatureDirection(tibia::Creature* creature, int direction)
    {
        sf::Vector2u tileCoords;

        tileCoords.x = -1;
        tileCoords.y = -1;

        switch (direction)
        {
            case tibia::Directions::up:
                if (creature->getY() == 0)
                {
                    break;
                }
                tileCoords.x = creature->getTileX();
                tileCoords.y = creature->getTileY() - tibia::TILE_SIZE;
                break;

            case tibia::Directions::right:
                if (creature->getX() == tibia::MAP_SIZE - 1)
                {
                    break;
                }
                tileCoords.x = creature->getTileX() + tibia::TILE_SIZE;
                tileCoords.y = creature->getTileY();
                break;

            case tibia::Directions::down:
                if (creature->getY() == tibia::MAP_SIZE - 1)
                {
                    break;
                }
                tileCoords.x = creature->getTileX();
                tileCoords.y = creature->getTileY() + tibia::TILE_SIZE;
                break;

            case tibia::Directions::left:
                if (creature->getX() == 0)
                {
                    break;
                }
                tileCoords.x = creature->getTileX() - tibia::TILE_SIZE;
                tileCoords.y = creature->getTileY();
                break;

            case tibia::Directions::upLeft:
                if (creature->getY() == 0 || creature->getX() == 0)
                {
                    break;
                }
                tileCoords.x = creature->getTileX() - tibia::TILE_SIZE;
                tileCoords.y = creature->getTileY() - tibia::TILE_SIZE;
                break;

            case tibia::Directions::upRight:
                if (creature->getY() == 0 || creature->getX() == tibia::MAP_SIZE - 1)
                {
                    break;
                }
                tileCoords.x = creature->getTileX() + tibia::TILE_SIZE;
                tileCoords.y = creature->getTileY() - tibia::TILE_SIZE;
                break;

            case tibia::Directions::downLeft:
                if (creature->getY() == tibia::MAP_SIZE - 1 || creature->getX() == 0)
                {
                    break;
                }
                tileCoords.x = creature->getTileX() - tibia::TILE_SIZE;
                tileCoords.y = creature->getTileY() + tibia::TILE_SIZE;
                break;

            case tibia::Directions::downRight:
                if (creature->getY() == tibia::MAP_SIZE - 1 || creature->getX() == tibia::MAP_SIZE - 1)
                {
                    break;
                }
                tileCoords.x = creature->getTileX() + tibia::TILE_SIZE;
                tileCoords.y = creature->getTileY() + tibia::TILE_SIZE;
                break;
        }

        return tileCoords;
    }

    bool checkMovementCollision(tibia::Creature* creature, int direction)
    {
        bool collision = false;

        sf::Vector2u checkTileCoords = getTileCoordsByCreatureDirection(creature, direction);

        if (checkTileCoords.x < 0 || checkTileCoords.y < 0)
        {
            return true;
        }

        int checkTileNumber = tibia::getTileNumberByTileCoords(checkTileCoords.x, checkTileCoords.y);

        if (creature->getZ() == tibia::ZAxis::aboveGround)
        {
            int checkTileId = m_map.tileMapAboveGroundTiles.getTilesList()->at(checkTileNumber)->getId();

            if (checkTileId == tibia::TILE_NULL)
            {
                return true;
            }
        }

        if (checkTileIsSolid(checkTileCoords, creature->getZ()) == true)
        {
            return true;
        }

        if (checkTileHasCreature(checkTileCoords, creature->getZ()) != nullptr)
        {
            return true;
        }

        if (creature->getZ() == m_player->getZ())
        {
            if (checkTileCoords.x == m_player->getTileX() && checkTileCoords.y == m_player->getTileY())
            {
                return true;
            }
        }

        return collision;
    }

    bool checkMovementMoveAboveOrBelow(tibia::Creature* creature, int direction)
    {
        sf::Vector2u checkTileCoords = getTileCoordsByCreatureDirection(creature, direction);

        if (checkTileCoords.x < 0 || checkTileCoords.y < 0)
        {
            return false;
        }

        if (checkTileIsMoveAbove(checkTileCoords, creature->getZ()) == true)
        {
            doCreatureMoveAbove(creature, checkTileCoords);

            return true;
        }

        if (checkTileIsMoveBelow(checkTileCoords, creature->getZ()) == true)
        {
            doCreatureMoveBelow(creature, checkTileCoords);

            return true;
        }

        return false;
    }

    bool checkMovementStepTile(tibia::Creature* creature, int direction, bool stepOn)
    {
        int checkTileNumber = 0;

        if (stepOn == true)
        {
            sf::Vector2u checkTileCoords = getTileCoordsByCreatureDirection(creature, direction);

            if (checkTileCoords.x < 0 || checkTileCoords.y < 0)
            {
                return false;
            }

            checkTileNumber = tibia::getTileNumberByTileCoords(checkTileCoords.x, checkTileCoords.y);
        }
        else
        {
            checkTileNumber = creature->getTileNumber();
        }

        if (checkTileNumber == 0)
        {
            return false;
        }

        tibia::TileMap* tileMap;

        switch (creature->getZ())
        {
            case tibia::ZAxis::aboveGround:
                tileMap = &m_map.tileMapAboveGroundTiles;
                break;

            case tibia::ZAxis::ground:
                tileMap = &m_map.tileMapGroundTiles;
                break;

            case tibia::ZAxis::underGround:
                tileMap = &m_map.tileMapUnderGroundTiles;
                break;
        }

        int checkTileId = tileMap->getTilesList()->at(checkTileNumber)->getId();

        int newTileId = 0;

        if (stepOn == true)
        {
            if (checkTileId == tibia::SpriteData::stepTileWood[0])
            {
                newTileId = tibia::SpriteData::stepTileWood[1];
            }
            else if (checkTileId == tibia::SpriteData::stepTileStone[0])
            {
                newTileId = tibia::SpriteData::stepTileStone[1];
            }
        }
        else
        {
            if (checkTileId == tibia::SpriteData::stepTileWood[1])
            {
                newTileId = tibia::SpriteData::stepTileWood[0];
            }
            else if (checkTileId == tibia::SpriteData::stepTileStone[1])
            {
                newTileId = tibia::SpriteData::stepTileStone[0];
            }
        }

        if (newTileId == 0)
        {
            return false;
        }

        tileMap->updateTileId(checkTileNumber, newTileId);

        return true;
    }

    bool doCreatureUseLever(tibia::Creature* creature, sf::Vector2u tilePosition)
    {
        int creatureDistanceFromTile = calculateDistanceByTile(creature->getTileX(), creature->getTileY(), tilePosition.x, tilePosition.y);

        //std::cout << "creatureDistanceFromTile: " << creatureDistanceFromTile << std::endl;

        if (creatureDistanceFromTile > 1)
        {
            return false;
        }

        tibia::TileMap* tileMap;

        switch (creature->getZ())
        {
            case tibia::ZAxis::aboveGround:
                tileMap = &m_map.tileMapAboveGroundObjects;
                break;

            case tibia::ZAxis::ground:
                tileMap = &m_map.tileMapGroundObjects;
                break;

            case tibia::ZAxis::underGround:
                tileMap = &m_map.tileMapUnderGroundObjects;
                break;
        }

        int tileNumber = tibia::getTileNumberByTileCoords(tilePosition.x, tilePosition.y);

        if (tileNumber < 0)
        {
            return false;
        }

        int checkTileId = tileMap->getTilesList()->at(tileNumber)->getId();

        int newTileId = 0;

        if (checkTileId == tibia::SpriteData::lever[0])
        {
            newTileId = tibia::SpriteData::lever[1];
        }
        else if (checkTileId == tibia::SpriteData::lever[1])
        {
            newTileId = tibia::SpriteData::lever[0];
        }

        if (newTileId == 0)
        {
            return false;
        }

        tileMap->updateTileId(tileNumber, newTileId);

        return true;
    }

    bool doCreatureUseLadder(tibia::Creature* creature, sf::Vector2u tilePosition)
    {
        int creatureDistanceFromTile = calculateDistanceByTile(creature->getTileX(), creature->getTileY(), tilePosition.x, tilePosition.y);

        //std::cout << "creatureDistanceFromTile: " << creatureDistanceFromTile << std::endl;

        if (creatureDistanceFromTile > 1)
        {
            return false;
        }

        if (checkTileIsLadder(tilePosition, creature->getZ()) == true)
        {
            doCreatureMoveAbove(creature, tilePosition);

            return true;
        }

        return false;
    }

    void doCreatureMoveAbove(tibia::Creature* creature, sf::Vector2u tilePosition)
    {
        int moveTileX = tilePosition.x - tibia::TILE_SIZE;
        int moveTileY = tilePosition.y - (tibia::TILE_SIZE * 2);

        int moveZ = creature->getZ() + 1;

        int moveDirection = tibia::Directions::up;

        if (checkTileIsNull(sf::Vector2u(moveTileX, moveTileY), moveZ) == true)
        {
            return;
        }

        if (checkTileIsSolid(sf::Vector2u(moveTileX, moveTileY), moveZ) == true)
        {
            moveTileY = tilePosition.y;

            moveDirection = tibia::Directions::down;

            if (checkTileIsSolid(sf::Vector2u(moveTileX, moveTileY), moveZ) == true)
            {
                return;
            }
        }

        creature->setTileCoords(moveTileX, moveTileY);

        creature->setZ(moveZ);

        creature->setDirection(moveDirection);
    }

    void doCreatureMoveBelow(tibia::Creature* creature, sf::Vector2u tilePosition)
    {
        int moveTileX = tilePosition.x + tibia::TILE_SIZE;
        int moveTileY = tilePosition.y + (tibia::TILE_SIZE * 2);

        int moveZ = creature->getZ() - 1;

        if (checkTileIsNull(sf::Vector2u(moveTileX, moveTileY), moveZ) == true)
        {
            return;
        }

        if (checkTileIsSolid(sf::Vector2u(moveTileX, moveTileY), moveZ) == true)
        {
            return;
        }

        creature->setTileCoords(moveTileX, moveTileY);

        creature->setZ(moveZ);

        creature->setDirection(tibia::Directions::down);
    }

    bool isPlayerNearWater()
    {
        int x = m_player->getX() - NUM_TILES_FROM_CENTER_X;
        int y = m_player->getY() - NUM_TILES_FROM_CENTER_Y;

        if (x < 0) x = 0;
        if (y < 0) y = 0;

        for (int i = x; i < x + tibia::NUM_TILES_X; i++)
        {
            for (int j = y; j < y + tibia::NUM_TILES_Y; j++)
            {
                if (i > tibia::MAP_SIZE - 1) continue;
                if (j > tibia::MAP_SIZE - 1) continue;

                int tileNumber = tibia::getTileNumberByTileCoords(i * tibia::TILE_SIZE, j * tibia::TILE_SIZE);

                if (tileNumber < 0 || tileNumber > tibia::TILE_NUMBER_MAX)
                {
                    continue;
                }

                if (m_map.tileMapGroundTiles.getTilesList()->at(tileNumber)->getFlags() & tibia::TileFlags::water)
                {
                    return true;
                }
            }
        }

        return false;
    }

    void drawShowVisibleTiles()
    {
        int x = m_player->getX() - NUM_TILES_FROM_CENTER_X;
        int y = m_player->getY() - NUM_TILES_FROM_CENTER_Y;

        if (x < 0) x = 0;
        if (y < 0) y = 0;

        for (int i = x; i < x + tibia::NUM_TILES_X; i++)
        {
            for (int j = y; j < y + tibia::NUM_TILES_Y; j++)
            {
                if (i > tibia::MAP_SIZE - 1) continue;
                if (j > tibia::MAP_SIZE - 1) continue;

                tibia::Sprite spr;
                spr.setId(1);
                spr.setColor(sf::Color(255, 0, 255, 64));
                spr.setPosition(sf::Vector2f(i * tibia::TILE_SIZE, j * tibia::TILE_SIZE));

                m_window.draw(spr);
            }
        }
    }

    int getCreatureTileNumber(tibia::Creature* creature)
    {
        return tibia::getTileNumberByTileCoords(creature->getTileX(), creature->getTileY());
    }

    float calculateDistanceBetweenThings(tibia::Thing* a, tibia::Thing* b)
    {
        //std::cout << a->getX() << "," << a->getY() << " | " << b->getX() << "," <<  b->getY() <<  std::endl;

        return tibia::calculateDistance(a->getX(), a->getY(), b->getX(), b->getY());
    }

    float calculateDistanceBetweenCreatures(tibia::Creature* a, tibia::Creature* b)
    {
        //std::cout << a->getX() << "," << a->getY() << " | " << b->getX() << "," <<  b->getY() <<  std::endl;

        return tibia::calculateDistance(a->getX(), a->getY(), b->getX(), b->getY());
    }

    std::vector<CreaturePtr>* getCreaturesList()
    {
        return &m_creaturesList;
    }

    std::vector<AnimationPtr>* getAnimationsList()
    {
        return &m_animationsList;
    }

    std::vector<AnimationPtr>* getAnimatedDecalsList()
    {
        return &m_animatedDecalsList;
    }

    std::vector<ProjectilePtr>* getProjectilesList()
    {
        return &m_projectilesList;
    }

    std::vector<SoundPtr>* getSoundList()
    {
        return &m_soundsList;
    }

    sf::Clock* getClock()
    {
        return &m_clock;
    }

    sf::Clock* getClockAnimatedWaterAndObjects()
    {
        return &m_clockAnimatedWaterAndObjects;
    }

    sf::Clock* getClockCreatureLogic()
    {
        return &m_clockCreatureLogic;
    }

    sf::Clock* getClockMiniMap()
    {
        return &m_clockMiniMap;
    }

    sf::RenderTexture* getWindow()
    {
        return &m_window;
    }

    sf::Sprite* getWindowSprite()
    {
        return &m_windowSprite;
    }

    sf::View* getWindowView()
    {
        return &m_windowView;
    }

    sf::Font* getFont()
    {
        return &m_font;
    }

    sf::Font* getFontSmall()
    {
        return &m_fontSmall;
    }

    sf::Font* getFontMartel()
    {
        return &m_fontMartel;
    }

    tibia::Map* getMap()
    {
        return &m_map;
    }

    tibia::Creature* getPlayer()
    {
        return m_player.get();
    }

private:

    sf::Clock m_clock;
    sf::Clock m_clockAnimatedWaterAndObjects;
    sf::Clock m_clockCreatureLogic;
    sf::Clock m_clockMiniMap;

    sf::RenderTexture m_window;
    sf::Sprite m_windowSprite;
    sf::RectangleShape m_windowBorder;
    sf::View m_windowView;

    sf::RenderTexture m_miniMapWindow;
    sf::Sprite m_miniMapWindowSprite;
    sf::RectangleShape m_miniMapWindowBorder;
    sf::View m_miniMapWindowView;

    sf::RenderTexture m_rtLight;
    sf::RectangleShape m_rectLight;

    sf::Font m_font;
    sf::Font m_fontSmall;
    sf::Font m_fontMartel;

    std::vector<sf::Text> m_textList;

    sf::Clock m_clockText;

    tibia::Map m_map;

    sf::VertexArray m_tileVertices;

    CreaturePtr m_player;

    std::vector<tibia::Thing*> m_thingsList;
    std::vector<tibia::Thing*> m_thingsSpawnList;

    std::vector<tibia::Map::ObjectPtr> m_objectsList;
    std::vector<tibia::Map::ObjectPtr> m_objectsSpawnList;

    std::vector<CreaturePtr> m_creaturesList;
    std::vector<CreaturePtr> m_creaturesSpawnList;

    std::vector<AnimationPtr> m_animationsList;
    std::vector<AnimationPtr> m_animationsSpawnList;

    std::vector<AnimationPtr> m_animatedDecalsList;
    std::vector<AnimationPtr> m_animatedDecalsSpawnList;

    std::vector<ProjectilePtr> m_projectilesList;
    std::vector<ProjectilePtr> m_projectilesSpawnList;

    std::vector<SoundPtr> m_soundsList;
    std::vector<SoundPtr> m_soundsSpawnList;
};

}

#endif // TIBIA_GAME_HPP
