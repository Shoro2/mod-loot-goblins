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
#include "CreatureScript.h"
#include "MotionMaster.h"
#include "Player.h"
#include "Random.h"
#include "ScriptedCreature.h"
#include "SpellAuras.h"
#include <algorithm>
#include <cmath>

// The Loot Goblin: never attacks, wanders quickly (pathfinding aware), carries
// a despawn debuff (handled by spell_loot_goblin_despawn) and has a chance to
// split into two fresh goblins when killed. A goblin that splits drops nothing.
class npc_loot_goblin : public CreatureScript
{
public:
    npc_loot_goblin() : CreatureScript("npc_loot_goblin") { }

    struct npc_loot_goblinAI : public ScriptedAI
    {
        explicit npc_loot_goblinAI(Creature* creature) : ScriptedAI(creature) { }

        void InitializeAI() override
        {
            LootGoblinsConfig const& c = gLootGoblinsCfg;

            // Never attacks. The creature stays attackable/killable because its
            // DB faction is neutral (it simply never retaliates).
            me->SetReactState(REACT_PASSIVE);

            // Fast, restless wandering. RandomMovementGenerator uses the
            // PathGenerator/mmaps, so the goblin will not run through walls.
            // Both walk and run speed are raised so it is fast regardless of
            // the move mode the generator picks.
            me->SetWalk(false);
            me->SetSpeed(MOVE_WALK, c.MoveSpeedRate, true);
            me->SetSpeed(MOVE_RUN, c.MoveSpeedRate, true);
            me->GetMotionMaster()->MoveRandom(c.WanderRadius);

            // Spawn debuff. The AuraScript despawns the goblin when this expires
            // (but not when it dies). Duration is forced here so it does not
            // depend on the DBC duration index.
            if (Aura* aura = me->AddAura(SPELL_LOOT_GOBLIN_DESPAWN, me))
            {
                aura->SetMaxDuration(int32(c.DebuffDurationMs));
                aura->SetDuration(int32(c.DebuffDurationMs));
            }
        }

        // Hard guarantees that the goblin never engages anything.
        void AttackStart(Unit* /*who*/) override { }
        void MoveInLineOfSight(Unit* /*who*/) override { }
        void UpdateAI(uint32 /*diff*/) override { }

        void JustDied(Unit* killer) override
        {
            LootGoblinsConfig const& c = gLootGoblinsCfg;

            // Base split chance plus the killer's split-talent bonus (flat
            // percentage points per stack, capped).
            float splitChance = c.SplitChance;
            if (Player* player = killer
                    ? killer->GetCharmerOrOwnerPlayerOrPlayerItself() : nullptr)
            {
                uint32 stacks = std::min<uint32>(
                    player->GetAuraCount(c.SplitTalentSpellId),
                    c.SplitTalentMaxStacks);
                splitChance += c.SplitTalentBonusPct * float(stacks);
            }

            if (!roll_chance_f(splitChance))
                return; // no split: the corpse keeps its normal loot

            // Spawn two fresh goblins near the corpse, placed via collision so
            // they do not end up inside geometry.
            for (uint8 i = 0; i < 2; ++i)
            {
                Position pos = me->GetPosition();
                me->MovePositionToFirstCollision(pos, frand(1.0f, 3.0f),
                    frand(0.0f, 2.0f * float(M_PI)));
                me->SummonCreature(c.GoblinEntry, pos,
                    TEMPSUMMON_CORPSE_TIMED_DESPAWN, c.CorpseDespawnMs);
            }

            // A goblin that splits drops nothing. Unit::Kill sets the LOOTABLE
            // dynamic flag right before JustDied is called, so removing it here
            // makes the corpse unlootable (Player::SendLoot checks this flag).
            me->loot.clear();
            me->RemoveDynamicFlag(UNIT_DYNFLAG_LOOTABLE);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_loot_goblinAI(creature);
    }
};

void AddSC_loot_goblin_npc()
{
    new npc_loot_goblin();
}
