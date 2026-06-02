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
#include "Creature.h"
#include "Map.h"
#include "Player.h"
#include "PlayerScript.h"
#include "Random.h"
#include <algorithm>

// Rolls for a Loot Goblin spawn whenever a player kills a creature. The chance
// depends on the victim's rank (normal/elite/boss) and the map type (open
// world / dungeon / raid), and is scaled by the killer's spawn talent.
class LootGoblins_KillScript : public PlayerScript
{
public:
    LootGoblins_KillScript()
        : PlayerScript("LootGoblins_KillScript",
            { PLAYERHOOK_ON_CREATURE_KILL }) { }

    void OnPlayerCreatureKill(Player* killer, Creature* killed) override
    {
        LootGoblinsConfig const& c = gLootGoblinsCfg;
        if (!c.Enable || !killer || !killed)
            return;

        // Never let a goblin's death roll another world spawn; the goblin AI
        // handles its own splitting.
        if (killed->GetEntry() == c.GoblinEntry)
            return;

        Map* map = killed->GetMap();
        if (!map)
            return;

        bool const boss = killed->IsDungeonBoss() || killed->isWorldBoss();
        bool const elite = killed->isElite();

        float chance;
        if (!map->Instanceable())          // open world
            chance = boss ? c.WorldBoss
                   : elite ? c.WorldElite : c.WorldNormal;
        else if (map->IsRaid())            // raid instance
            chance = boss ? c.RaidBoss
                   : elite ? c.RaidElite : c.RaidNormal;
        else if (map->IsDungeon())         // 5-man dungeon
            chance = boss ? c.DungeonBoss
                   : elite ? c.DungeonElite : c.DungeonNormal;
        else
            return;                        // battlegrounds / arenas: no spawn

        if (chance <= 0.0f)
            return;

        // Spawn talent: chance * (1 + Bonus/100 * stacks), stacks capped.
        if (uint32 stacks = std::min<uint32>(
                killer->GetAuraCount(c.SpawnTalentSpellId),
                c.SpawnTalentMaxStacks))
            chance *= (1.0f + (c.SpawnTalentBonusPct / 100.0f) * float(stacks));

        if (!roll_chance_f(chance))
            return;

        killer->SummonCreature(c.GoblinEntry, killed->GetPosition(),
            TEMPSUMMON_CORPSE_TIMED_DESPAWN, c.CorpseDespawnMs);
    }
};

void AddSC_loot_goblin_world()
{
    new LootGoblins_KillScript();
}
