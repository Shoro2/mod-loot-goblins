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

#include "LootGoblins.h"
#include "Config.h"
#include "WorldScript.h"

// The single shared config instance, populated on (re)load.
LootGoblinsConfig gLootGoblinsCfg;

// Loads the module configuration whenever the server (re)reads its config.
class LootGoblins_ConfigWorldScript : public WorldScript
{
public:
    LootGoblins_ConfigWorldScript()
        : WorldScript("LootGoblins_ConfigWorldScript",
            { WORLDHOOK_ON_AFTER_CONFIG_LOAD }) { }

    void OnAfterConfigLoad(bool /*reload*/) override
    {
        LootGoblinsConfig& c = gLootGoblinsCfg;

        c.Enable = sConfigMgr->GetOption<bool>(
            "LootGoblins.Enable", true);
        c.GoblinEntry = sConfigMgr->GetOption<uint32>(
            "LootGoblins.GoblinEntry", 400000);
        c.DebuffDurationMs = sConfigMgr->GetOption<uint32>(
            "LootGoblins.DebuffDurationMs", 10000);
        c.CorpseDespawnMs = sConfigMgr->GetOption<uint32>(
            "LootGoblins.CorpseDespawnMs", 30000);
        c.MoveSpeedRate = sConfigMgr->GetOption<float>(
            "LootGoblins.MoveSpeedRate", 2.5f);
        c.WanderRadius = sConfigMgr->GetOption<float>(
            "LootGoblins.WanderRadius", 8.0f);
        c.SplitChance = sConfigMgr->GetOption<float>(
            "LootGoblins.SplitChance", 5.0f);

        c.SpawnTalentSpellId = sConfigMgr->GetOption<uint32>(
            "LootGoblins.SpawnTalent.SpellId", 700002);
        c.SpawnTalentBonusPct = sConfigMgr->GetOption<float>(
            "LootGoblins.SpawnTalent.BonusPerStack", 50.0f);
        c.SpawnTalentMaxStacks = sConfigMgr->GetOption<uint32>(
            "LootGoblins.SpawnTalent.MaxStacks", 6);

        c.SplitTalentSpellId = sConfigMgr->GetOption<uint32>(
            "LootGoblins.SplitTalent.SpellId", 700003);
        c.SplitTalentBonusPct = sConfigMgr->GetOption<float>(
            "LootGoblins.SplitTalent.BonusPerStack", 5.0f);
        c.SplitTalentMaxStacks = sConfigMgr->GetOption<uint32>(
            "LootGoblins.SplitTalent.MaxStacks", 6);

        c.WorldNormal = sConfigMgr->GetOption<float>(
            "LootGoblins.Chance.World.Normal", 0.01f);
        c.WorldElite = sConfigMgr->GetOption<float>(
            "LootGoblins.Chance.World.Elite", 0.05f);
        c.WorldBoss = sConfigMgr->GetOption<float>(
            "LootGoblins.Chance.World.Boss", 1.0f);
        c.DungeonNormal = sConfigMgr->GetOption<float>(
            "LootGoblins.Chance.Dungeon.Normal", 0.05f);
        c.DungeonElite = sConfigMgr->GetOption<float>(
            "LootGoblins.Chance.Dungeon.Elite", 0.1f);
        c.DungeonBoss = sConfigMgr->GetOption<float>(
            "LootGoblins.Chance.Dungeon.Boss", 0.5f);
        c.RaidNormal = sConfigMgr->GetOption<float>(
            "LootGoblins.Chance.Raid.Normal", 0.1f);
        c.RaidElite = sConfigMgr->GetOption<float>(
            "LootGoblins.Chance.Raid.Elite", 0.2f);
        c.RaidBoss = sConfigMgr->GetOption<float>(
            "LootGoblins.Chance.Raid.Boss", 1.0f);
    }
};

// Per-file script registrators (defined in their respective .cpp files).
void AddSC_loot_goblin_npc();
void AddSC_loot_goblin_world();
void AddSC_loot_goblin_spell();

// Module entry point. The name is derived by the module loader from the
// directory name "mod-loot-goblins" (see modules/CMakeLists.txt: '-' -> '_').
void Addmod_loot_goblinsScripts()
{
    new LootGoblins_ConfigWorldScript();
    AddSC_loot_goblin_npc();
    AddSC_loot_goblin_world();
    AddSC_loot_goblin_spell();
}
