/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MODULE_LOOT_GOBLINS_H
#define MODULE_LOOT_GOBLINS_H

#include "Define.h"

// Fixed custom IDs. These MUST match the values shipped in
// data/sql/db-world/base/*.sql (spell_dbc + spell_script_names).
enum LootGoblinSpells
{
    SPELL_LOOT_GOBLIN_DESPAWN = 700001  // server-side debuff (spell_dbc row)
    // 700002 = spawn-chance talent, 700003 = split-chance talent.
    // Their IDs are configurable below (SpawnTalentSpellId/SplitTalentSpellId).
};

// All tunables live here and are (re)loaded from loot_goblins.conf via the
// WorldScript hook OnAfterConfigLoad. The in-class defaults keep the module
// safe even if a kill happens before the config is loaded.
struct LootGoblinsConfig
{
    bool   Enable           = true;
    uint32 GoblinEntry      = 400000;
    uint32 DebuffDurationMs = 10000;   // how long a living goblin survives
    uint32 CorpseDespawnMs  = 30000;   // how long the lootable corpse lingers
    float  MoveSpeedRate    = 2.5f;    // movement speed multiplier (fast)
    float  WanderRadius     = 8.0f;    // MoveRandom wander radius (yards)
    float  SplitChance      = 5.0f;    // base % chance to split into 2 on death

    // Spawn talent: admin-granted, stackable aura that multiplies the per-kill
    // spawn chance. Bonus is applied as chance * (1 + Bonus/100 * stacks),
    // e.g. 50% per stack => x4 at 6 stacks.
    uint32 SpawnTalentSpellId  = 700002;
    float  SpawnTalentBonusPct = 50.0f;
    uint32 SpawnTalentMaxStacks = 6;

    // Split talent: admin-granted, stackable aura that adds flat percentage
    // points to the split chance, e.g. base 5% + 5 points per stack => 35% at
    // 6 stacks. The buff is read from the player who lands the killing blow.
    uint32 SplitTalentSpellId  = 700003;
    float  SplitTalentBonusPct = 5.0f;
    uint32 SplitTalentMaxStacks = 6;

    // Per-kill spawn chances in percent. Defaults are 1/10 of the "full"
    // tuning so the feature is rare until the talent ramps it up.
    float WorldNormal   = 0.01f;
    float WorldElite    = 0.05f;
    float WorldBoss     = 1.0f;
    float DungeonNormal = 0.05f;
    float DungeonElite  = 0.1f;
    float DungeonBoss   = 0.5f;
    float RaidNormal    = 0.1f;
    float RaidElite     = 0.2f;
    float RaidBoss      = 1.0f;
};

extern LootGoblinsConfig gLootGoblinsCfg;

#endif // MODULE_LOOT_GOBLINS_H
