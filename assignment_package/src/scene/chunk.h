#pragma once
#include "smartpointerhelp.h"
#include "glm_includes.h"
#include "drawable.h"

#include <array>
#include <unordered_map>
#include <cstddef>


// Lets us use any enum class as the key of a
// std::unordered_map
struct EnumHash {
    template <typename T>
    size_t operator()(T t) const {
        return static_cast<size_t>(t);
    }
};

enum BlockType : unsigned char
{
    EMPTY, GRASS, DIRT, STONE, WATER, SNOW, LAVA, BEDROCK, SAND, ORANGE_ROCK, RED_ROCK,
    DARKNESS, BLACK_ROCK, TORCH, LIGHT_LEAVES, DARK_LEAVES, LIGHT_WOOD, MEDIUM_WOOD,
    DARK_WOOD, ICE, FAT_BUSH, SKINNY_BUSH, DESERT_BUSH, LIGHT_GRASS, SHORT_GRASS, MEDIUM_GRASS,
    CACTUS, YELLOW_FLOWER, RED_FLOWER, WINTER_TREE, MUD, MUSHROOM_STEM, BLUE_MUSHROOM,
    RED_MUSHROOM, YELLOW_MUSHROOM, TEAL_MUSHROOM
};

// we want to treat any blocks set to true in this map as "empty" blocks.
// so that in our createMultithreaded code, neighbouring faces should be rendered.
// this is generally blocks with transparency / alpha channels
static std::unordered_map<BlockType, bool, EnumHash> hasAlpha = {
    {EMPTY, false},
    {GRASS, false},
    {DIRT, false},
    {STONE, false},
    {WATER, true},
    {SNOW, false},
    {LAVA, true},
    {BEDROCK, false},
    {SAND, false},
    {ORANGE_ROCK, false},
    {RED_ROCK, false},
    {DARKNESS, false},
    {BLACK_ROCK, false},
    {TORCH, true},
    {LIGHT_LEAVES, false},
    {DARK_LEAVES, false},
    {LIGHT_WOOD, false},
    {MEDIUM_WOOD, false},
    {DARK_WOOD, false},
    {ICE, false},
    {FAT_BUSH, true},
    {SKINNY_BUSH, true},
    {DESERT_BUSH, true},
    {LIGHT_GRASS, true},
    {SHORT_GRASS, true},
    {MEDIUM_GRASS, true},
    {CACTUS, false},
    {YELLOW_FLOWER, true},
    {RED_FLOWER, true},
    {WINTER_TREE, true},
    {MUD, false},
    {MUSHROOM_STEM, false},
    {BLUE_MUSHROOM, false},
    {RED_MUSHROOM, false},
    {YELLOW_MUSHROOM, false},
    {TEAL_MUSHROOM, false}
};

// The six cardinal directions in 3D space
enum Direction : unsigned char
{
    XPOS, XNEG, YPOS, YNEG, ZPOS, ZNEG
};


const static std::unordered_map<BlockType, std::unordered_map<Direction, glm::vec2, EnumHash>, EnumHash> blockFaceUVs {
    {GRASS, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(3.f/16.f, 15.f/16.f)}, //right side of the grass
                                                               {XNEG, glm::vec2(3.f/16.f, 15.f/16.f)}, //left side of the grass
                                                               {YPOS, glm::vec2(8.f/16.f, 13.f/16.f)}, //top of grass
                                                               {YNEG, glm::vec2(2.f/16.f, 15.f/16.f)}, //bottom of grass
                                                               {ZPOS, glm::vec2(3.f/16.f, 15.f/16.f)}, //front side of grass
                                                               {ZNEG, glm::vec2(3.f/16.f, 15.f/16.f)}}}, //back side of grass

    {DIRT, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(2.f/16.f, 15.f/16.f)},
                                                              {XNEG, glm::vec2(2.f/16.f, 15.f/16.f)},
                                                              {YPOS, glm::vec2(2.f/16.f, 15.f/16.f)},
                                                              {YNEG, glm::vec2(2.f/16.f, 15.f/16.f)},
                                                              {ZPOS, glm::vec2(2.f/16.f, 15.f/16.f)},
                                                              {ZNEG, glm::vec2(2.f/16.f, 15.f/16.f)}}},

    {STONE, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(1.f/16.f, 15.f/16.f)},
                                                               {XNEG, glm::vec2(1.f/16.f, 15.f/16.f)},
                                                               {YPOS, glm::vec2(1.f/16.f, 15.f/16.f)},
                                                               {YNEG, glm::vec2(1.f/16.f, 15.f/16.f)},
                                                               {ZPOS, glm::vec2(1.f/16.f, 15.f/16.f)},
                                                               {ZNEG, glm::vec2(1.f/16.f, 15.f/16.f)}}},

    {LAVA, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(15.f/16.f, 1.f/16.f)},
                                                              {XNEG, glm::vec2(15.f/16.f, 1.f/16.f)},
                                                              {YPOS, glm::vec2(15.f/16.f, 1.f/16.f)},
                                                              {YNEG, glm::vec2(15.f/16.f, 1.f/16.f)},
                                                              {ZPOS, glm::vec2(15.f/16.f, 1.f/16.f)},
                                                              {ZNEG, glm::vec2(15.f/16.f, 1.f/16.f)}}},

    {WATER, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(15.f/16.f, 3.f/16.f)},
                                                               {XNEG, glm::vec2(15.f/16.f, 3.f/16.f)},
                                                               {YPOS, glm::vec2(15.f/16.f, 3.f/16.f)},
                                                               {YNEG, glm::vec2(15.f/16.f, 3.f/16.f)},
                                                               {ZPOS, glm::vec2(15.f/16.f, 3.f/16.f)},
                                                               {ZNEG, glm::vec2(15.f/16.f, 3.f/16.f)}}},

    {SNOW, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(2.f/16.f, 11.f/16.f)},
                                                              {XNEG, glm::vec2(2.f/16.f, 11.f/16.f)},
                                                              {YPOS, glm::vec2(2.f/16.f, 11.f/16.f)},
                                                              {YNEG, glm::vec2(2.f/16.f, 11.f/16.f)},
                                                              {ZPOS, glm::vec2(2.f/16.f, 11.f/16.f)},
                                                              {ZNEG, glm::vec2(2.f/16.f, 11.f/16.f)}}},

    {SAND, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(2.f/16.f, 14.f/16.f)},
                                                              {XNEG, glm::vec2(2.f/16.f, 14.f/16.f)},
                                                              {YPOS, glm::vec2(2.f/16.f, 14.f/16.f)},
                                                              {YNEG, glm::vec2(2.f/16.f, 14.f/16.f)},
                                                              {ZPOS, glm::vec2(2.f/16.f, 14.f/16.f)},
                                                              {ZNEG, glm::vec2(2.f/16.f, 14.f/16.f)}}},

    {BEDROCK, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(1.f/16.f, 14.f/16.f)},
                                                                 {XNEG, glm::vec2(1.f/16.f, 14.f/16.f)},
                                                                 {YPOS, glm::vec2(1.f/16.f, 14.f/16.f)},
                                                                 {YNEG, glm::vec2(1.f/16.f, 14.f/16.f)},
                                                                 {ZPOS, glm::vec2(1.f/16.f, 14.f/16.f)},
                                                                 {ZNEG, glm::vec2(1.f/16.f, 14.f/16.f)}}},

    {ORANGE_ROCK, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(2.f/16.f, 2.f/16.f)},
                                                                    {XNEG, glm::vec2(2.f/16.f, 2.f/16.f)},
                                                                    {YPOS, glm::vec2(2.f/16.f, 2.f/16.f)},
                                                                    {YNEG, glm::vec2(2.f/16.f, 2.f/16.f)},
                                                                    {ZPOS, glm::vec2(2.f/16.f, 2.f/16.f)},
                                                                    {ZNEG, glm::vec2(2.f/16.f, 2.f/16.f)}}},

    {RED_ROCK, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(1.f/16.f, 7.f/16.f)},
                                                                 {XNEG, glm::vec2(1.f/16.f, 7.f/16.f)},
                                                                 {YPOS, glm::vec2(1.f/16.f, 7.f/16.f)},
                                                                 {YNEG, glm::vec2(1.f/16.f, 7.f/16.f)},
                                                                 {ZPOS, glm::vec2(1.f/16.f, 7.f/16.f)},
                                                                 {ZNEG, glm::vec2(1.f/16.f, 7.f/16.f)}}},

    {DARKNESS, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(5.f/16.f, 13.f/16.f)},
                                                                  {XNEG, glm::vec2(5.f/16.f, 13.f/16.f)},
                                                                  {YPOS, glm::vec2(5.f/16.f, 13.f/16.f)},
                                                                  {YNEG, glm::vec2(5.f/16.f, 13.f/16.f)},
                                                                  {ZPOS, glm::vec2(5.f/16.f, 13.f/16.f)},
                                                                  {ZNEG, glm::vec2(5.f/16.f, 13.f/16.f)}}},

    {BLACK_ROCK, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(2.f/16.f, 8.f/16.f)},
                                                                   {XNEG, glm::vec2(2.f/16.f, 8.f/16.f)},
                                                                   {YPOS, glm::vec2(2.f/16.f, 8.f/16.f)},
                                                                   {YNEG, glm::vec2(2.f/16.f, 8.f/16.f)},
                                                                   {ZPOS, glm::vec2(2.f/16.f, 8.f/16.f)},
                                                                   {ZNEG, glm::vec2(2.f/16.f, 8.f/16.f)}}},

    {TORCH, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(3.f/16.f, 9.f/16.f)},
                                                               {XNEG, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                               {YPOS, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                               {YNEG, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                               {ZPOS, glm::vec2(3.f/16.f, 9.f/16.f)},
                                                               {ZNEG, glm::vec2(4.f/16.f, 4.f/16.f)}}},

    {LIGHT_LEAVES, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(4.f/16.f, 12.f/16.f)},
                                                                      {XNEG, glm::vec2(4.f/16.f, 12.f/16.f)},
                                                                      {YPOS, glm::vec2(4.f/16.f, 12.f/16.f)},
                                                                      {YNEG, glm::vec2(4.f/16.f, 12.f/16.f)},
                                                                      {ZPOS, glm::vec2(4.f/16.f, 12.f/16.f)},
                                                                      {ZNEG, glm::vec2(4.f/16.f, 12.f/16.f)}}},

    {DARK_LEAVES, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(5.f/16.f, 12.f/16.f)},
                                                                     {XNEG, glm::vec2(5.f/16.f, 12.f/16.f)},
                                                                     {YPOS, glm::vec2(5.f/16.f, 12.f/16.f)},
                                                                     {YNEG, glm::vec2(5.f/16.f, 12.f/16.f)},
                                                                     {ZPOS, glm::vec2(5.f/16.f, 12.f/16.f)},
                                                                     {ZNEG, glm::vec2(5.f/16.f, 12.f/16.f)}}},

    {LIGHT_WOOD, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(7.f/16.f, 10.f/16.f)},
                                                                    {XNEG, glm::vec2(7.f/16.f, 10.f/16.f)},
                                                                    {YPOS, glm::vec2(7.f/16.f, 10.f/16.f)},
                                                                    {YNEG, glm::vec2(7.f/16.f, 10.f/16.f)},
                                                                    {ZPOS, glm::vec2(7.f/16.f, 10.f/16.f)},
                                                                    {ZNEG, glm::vec2(7.f/16.f, 10.f/16.f)}}},

    {MEDIUM_WOOD, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(6.f/16.f, 10.f/16.f)},
                                                                     {XNEG, glm::vec2(6.f/16.f, 10.f/16.f)},
                                                                     {YPOS, glm::vec2(6.f/16.f, 10.f/16.f)},
                                                                     {YNEG, glm::vec2(6.f/16.f, 10.f/16.f)},
                                                                     {ZPOS, glm::vec2(6.f/16.f, 10.f/16.f)},
                                                                     {ZNEG, glm::vec2(6.f/16.f, 10.f/16.f)}}},

    {DARK_WOOD, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(4.f/16.f, 8.f/16.f)},
                                                                   {XNEG, glm::vec2(4.f/16.f, 8.f/16.f)},
                                                                   {YPOS, glm::vec2(4.f/16.f, 8.f/16.f)},
                                                                   {YNEG, glm::vec2(4.f/16.f, 8.f/16.f)},
                                                                   {ZPOS, glm::vec2(4.f/16.f, 8.f/16.f)},
                                                                   {ZNEG, glm::vec2(4.f/16.f, 8.f/16.f)}}},

    {ICE, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(3.f/16.f, 11.f/16.f)},
                                                             {XNEG, glm::vec2(3.f/16.f, 11.f/16.f)},
                                                             {YPOS, glm::vec2(3.f/16.f, 11.f/16.f)},
                                                             {YNEG, glm::vec2(3.f/16.f, 11.f/16.f)},
                                                             {ZPOS, glm::vec2(3.f/16.f, 11.f/16.f)},
                                                             {ZNEG, glm::vec2(3.f/16.f, 11.f/16.f)}}},

    {FAT_BUSH, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(15.f/16.f, 15.f/16.f)},
                                                                  {XNEG, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                                  {YPOS, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                                  {YNEG, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                                  {ZPOS, glm::vec2(15.f/16.f, 15.f/16.f)},
                                                                  {ZNEG, glm::vec2(4.f/16.f, 4.f/16.f)}}},

    {SKINNY_BUSH, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(14.f/16.f, 14.f/16.f)},
                                                                     {XNEG, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                                     {YPOS, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                                     {YNEG, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                                     {ZPOS, glm::vec2(14.f/16.f, 14.f/16.f)},
                                                                     {ZNEG, glm::vec2(4.f/16.f, 4.f/16.f)}}},

    {DESERT_BUSH, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(15.f/16.f, 12.f/16.f)},
                                                                     {XNEG, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                                     {YPOS, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                                     {YNEG, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                                     {ZPOS, glm::vec2(15.f/16.f, 12.f/16.f)},
                                                                     {ZNEG, glm::vec2(4.f/16.f, 4.f/16.f)}}},

    {LIGHT_GRASS, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(7.f/16.f, 13.f/16.f)},
                                                                     {XNEG, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                                     {YPOS, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                                     {YNEG, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                                     {ZPOS, glm::vec2(7.f/16.f, 13.f/16.f)},
                                                                     {ZNEG, glm::vec2(4.f/16.f, 4.f/16.f)}}},

    {SHORT_GRASS, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(11.f/16.f, 10.f/16.f)},
                                                                     {XNEG, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                                     {YPOS, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                                     {YNEG, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                                     {ZPOS, glm::vec2(11.f/16.f, 10.f/16.f)},
                                                                     {ZNEG, glm::vec2(4.f/16.f, 4.f/16.f)}}},

    {MEDIUM_GRASS, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(13.f/16.f, 10.f/16.f)},
                                                                      {XNEG, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                                      {YPOS, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                                      {YNEG, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                                      {ZPOS, glm::vec2(13.f/16.f, 10.f/16.f)},
                                                                      {ZNEG, glm::vec2(4.f/16.f, 4.f/16.f)}}},

    {CACTUS, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(6.f/16.f, 11.f/16.f)},
                                                                {XNEG, glm::vec2(6.f/16.f, 11.f/16.f)},
                                                                {YPOS, glm::vec2(5.f/16.f, 11.f/16.f)},
                                                                {YNEG, glm::vec2(6.f/16.f, 11.f/16.f)},
                                                                {ZPOS, glm::vec2(6.f/16.f, 11.f/16.f)},
                                                                {ZNEG, glm::vec2(6.f/16.f, 11.f/16.f)}}},

    {YELLOW_FLOWER, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(13.f/16.f, 15.f/16.f)},
                                                                       {XNEG, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                                       {YPOS, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                                       {YNEG, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                                       {ZPOS, glm::vec2(13.f/16.f, 15.f/16.f)},
                                                                       {ZNEG, glm::vec2(4.f/16.f, 4.f/16.f)}}},

    {RED_FLOWER, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(12.f/16.f, 15.f/16.f)},
                                                                    {XNEG, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                                    {YPOS, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                                    {YNEG, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                                    {ZPOS, glm::vec2(12.f/16.f, 15.f/16.f)},
                                                                    {ZNEG, glm::vec2(4.f/16.f, 4.f/16.f)}}},

    {WINTER_TREE, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(7.f/16.f, 12.f/16.f)},
                                                                     {XNEG, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                                     {YPOS, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                                     {YNEG, glm::vec2(4.f/16.f, 4.f/16.f)},
                                                                     {ZPOS, glm::vec2(7.f/16.f, 12.f/16.f)},
                                                                     {ZNEG, glm::vec2(4.f/16.f, 4.f/16.f)}}},

    {MUD, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(8.f/16.f, 9.f/16.f)},
                                                             {XNEG, glm::vec2(8.f/16.f, 9.f/16.f)},
                                                             {YPOS, glm::vec2(8.f/16.f, 9.f/16.f)},
                                                             {YNEG, glm::vec2(8.f/16.f, 9.f/16.f)},
                                                             {ZPOS, glm::vec2(8.f/16.f, 9.f/16.f)},
                                                             {ZNEG, glm::vec2(8.f/16.f, 9.f/16.f)}}},

    {MUSHROOM_STEM, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(13.f/16.f, 7.f/16.f)},
                                                                       {XNEG, glm::vec2(13.f/16.f, 7.f/16.f)},
                                                                       {YPOS, glm::vec2(13.f/16.f, 7.f/16.f)},
                                                                       {YNEG, glm::vec2(13.f/16.f, 7.f/16.f)},
                                                                       {ZPOS, glm::vec2(13.f/16.f, 7.f/16.f)},
                                                                       {ZNEG, glm::vec2(13.f/16.f, 7.f/16.f)}}},

    {BLUE_MUSHROOM, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(0.f/16.f, 6.f/16.f)},
                                                                       {XNEG, glm::vec2(0.f/16.f, 6.f/16.f)},
                                                                       {YPOS, glm::vec2(0.f/16.f, 6.f/16.f)},
                                                                       {YNEG, glm::vec2(0.f/16.f, 6.f/16.f)},
                                                                       {ZPOS, glm::vec2(0.f/16.f, 6.f/16.f)},
                                                                       {ZNEG, glm::vec2(0.f/16.f, 6.f/16.f)}}},

    {BLUE_MUSHROOM, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(0.f/16.f, 6.f/16.f)},
                                                                       {XNEG, glm::vec2(0.f/16.f, 6.f/16.f)},
                                                                       {YPOS, glm::vec2(0.f/16.f, 6.f/16.f)},
                                                                       {YNEG, glm::vec2(0.f/16.f, 6.f/16.f)},
                                                                       {ZPOS, glm::vec2(0.f/16.f, 6.f/16.f)},
                                                                       {ZNEG, glm::vec2(0.f/16.f, 6.f/16.f)}}},

    {RED_MUSHROOM, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(13.f/16.f, 8.f/16.f)},
                                                                      {XNEG, glm::vec2(13.f/16.f, 8.f/16.f)},
                                                                      {YPOS, glm::vec2(13.f/16.f, 8.f/16.f)},
                                                                      {YNEG, glm::vec2(13.f/16.f, 8.f/16.f)},
                                                                      {ZPOS, glm::vec2(13.f/16.f, 8.f/16.f)},
                                                                      {ZNEG, glm::vec2(13.f/16.f, 8.f/16.f)}}},

    {YELLOW_MUSHROOM, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(0.f/16.f, 12.f/16.f)},
                                                                         {XNEG, glm::vec2(0.f/16.f, 12.f/16.f)},
                                                                         {YPOS, glm::vec2(0.f/16.f, 12.f/16.f)},
                                                                         {YNEG, glm::vec2(0.f/16.f, 12.f/16.f)},
                                                                         {ZPOS, glm::vec2(0.f/16.f, 12.f/16.f)},
                                                                         {ZNEG, glm::vec2(0.f/16.f, 12.f/16.f)}}},

    {TEAL_MUSHROOM, std::unordered_map<Direction, glm::vec2, EnumHash>{{XPOS, glm::vec2(7.f/16.f, 9.f/16.f)},
                                                                       {XNEG, glm::vec2(7.f/16.f, 9.f/16.f)},
                                                                       {YPOS, glm::vec2(7.f/16.f, 9.f/16.f)},
                                                                       {YNEG, glm::vec2(7.f/16.f, 9.f/16.f)},
                                                                       {ZPOS, glm::vec2(7.f/16.f, 9.f/16.f)},
                                                                       {ZNEG, glm::vec2(7.f/16.f, 9.f/16.f)}}},
};

class Chunk;

struct ChunkVBOData {
    Chunk* chunk;
    std::vector<GLfloat> vboDataOpaque, vboDataTransparent;
    std::vector<GLuint> idxDataOpaque, idxDataTransparent;

    ChunkVBOData(Chunk *c) : chunk(c),
        vboDataOpaque{}, vboDataTransparent{},
        idxDataOpaque{}, idxDataTransparent{}
    {}
};

// One Chunk is a 16 x 256 x 16 section of the world,
// containing all the Minecraft blocks in that area.
// We divide the world into Chunks in order to make
// recomputing its VBO data faster by not having to
// render all the world at once, while also not having
// to render the world block by block.

class Chunk : public Drawable {
private:
    // All of the blocks contained within this Chunk
    std::array<BlockType, 65536> m_blocks;
    int minX, minZ;
    // This Chunk's four neighbors to the north, south, east, and west
    // The third input to this map just lets us use a Direction as
    // a key for this map.
    // These allow us to properly determine
    std::unordered_map<Direction, Chunk*, EnumHash> m_neighbors;

public:
    Chunk(OpenGLContext* context, int x, int z);
    BlockType getBlockAt(unsigned int x, unsigned int y, unsigned int z) const;
    BlockType getBlockAt(int x, int y, int z) const;
    void setBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t);
    void linkNeighbor(uPtr<Chunk>& neighbor, Direction dir);
    void createVBOBuffer(std::vector<GLfloat>& vertexDataOpaque, std::vector<GLfloat>& vertexDataTransparent,
                         std::vector<GLuint> &idxDataOpaque, std::vector<GLuint> &idxDataTransparent);
    bool isBlockAnimateable(BlockType type);
    Direction convertToDirection(const glm::ivec3& dirVec);

    glm::vec2 calculateUV(BlockType current, glm::ivec3 direction);

    // Drawable interface
public:
    // fills `data` with vertex and index data necessary to create a VBO
    void createMultithreaded(ChunkVBOData &data);
    void createVBOdata() override;
    GLenum drawMode() override { return GL_TRIANGLES; }
    std::unordered_map<Direction, Chunk *, EnumHash>& neighbors();
    int getMinX() const;
    int getMinZ() const;
};
