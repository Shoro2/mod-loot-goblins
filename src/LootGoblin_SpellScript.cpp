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

#include "Creature.h"
#include "SpellAuraEffects.h"
#include "SpellAuras.h"
#include "SpellScript.h"
#include "SpellScriptLoader.h"

// Attached to the goblin's spawn debuff via spell_script_names. When the debuff
// expires, the (still living) goblin despawns. If the goblin was killed instead,
// the aura is removed by death, so we leave the lootable corpse alone.
class spell_loot_goblin_despawn : public AuraScript
{
    PrepareAuraScript(spell_loot_goblin_despawn);

    void HandleEffectRemove(AuraEffect const* /*aurEff*/,
        AuraEffectHandleModes /*mode*/)
    {
        if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
            return;

        if (Creature* goblin = GetTarget()->ToCreature())
            if (goblin->IsAlive())
                goblin->DespawnOrUnsummon();
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(
            spell_loot_goblin_despawn::HandleEffectRemove,
            EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

void AddSC_loot_goblin_spell()
{
    RegisterSpellScript(spell_loot_goblin_despawn);
}
