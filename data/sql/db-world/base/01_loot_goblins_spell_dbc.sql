-- Loot Goblins module - custom server-side spells
--
-- AzerothCore merges the `spell_dbc` table into the server spell store on top
-- of Spell.dbc (see DBCStores.cpp: LOAD_DBC(sSpellStore, "Spell.dbc",
-- "spell_dbc")), so these IDs resolve server-side without touching the client
-- DBC. They are invisible/dummy auras; that is fine because:
--   * 700001 is an NPC self-debuff used only as a despawn timer, and
--   * 700002/700003 are read by the server via GetAuraCount().
--
-- DurationIndex 21 = infinite (-1) in 3.3.5a SpellDuration.dbc, which keeps the
-- talents permanent. The 700001 debuff's real 10s duration is forced in code
-- (SetMaxDuration/SetDuration), so its DurationIndex does not matter.
--
-- Effect_1 = 6 (SPELL_EFFECT_APPLY_AURA), EffectAura_1 = 4 (SPELL_AURA_DUMMY),
-- ImplicitTargetA_1 = 1 (TARGET_UNIT_CASTER), CumulativeAura = max stacks.

DELETE FROM `spell_dbc` WHERE `ID` IN (700001, 700002, 700003);
INSERT INTO `spell_dbc`
(`ID`,`Attributes`,`DurationIndex`,`RangeIndex`,`CumulativeAura`,
 `Effect_1`,`EffectAura_1`,`ImplicitTargetA_1`,`SchoolMask`,`SpellIconID`,
 `Name_Lang_enUS`) VALUES
(700001,0,21,1,0,6,4,1,1,1,'Loot Goblin Despawn Timer'),
(700002,0,21,1,6,6,4,1,1,1,'Loot Goblin Fortune (Spawn Chance)'),
(700003,0,21,1,6,6,4,1,1,1,'Loot Goblin Greed (Split Chance)');
