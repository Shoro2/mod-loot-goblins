-- Loot Goblins module - world database data
-- Creates the Loot Goblin creature, its loot and the spell script binding.

-- ---------------------------------------------------------------------------
-- Creature template
--   faction 7  : neutral -> players can attack/kill it, but it never aggros
--   type 7     : humanoid
--   MovementType 0: the script drives MoveRandom (works for summons too)
--   HealthModifier 0.2 + RegenHealth 0: squishy and killable within ~10s
--   ScriptName : binds the npc_loot_goblin CreatureScript / AI
-- ---------------------------------------------------------------------------
DELETE FROM `creature_template` WHERE `entry` = 400000;
INSERT INTO `creature_template`
(`entry`,`name`,`subname`,`minlevel`,`maxlevel`,`exp`,`faction`,`npcflag`,
 `scale`,`rank`,`unit_class`,`type`,`lootid`,`mingold`,`maxgold`,
 `MovementType`,`HealthModifier`,`RegenHealth`,`flags_extra`,`ScriptName`) VALUES
(400000,'Loot Goblin','Treasure Hoarder',80,80,2,7,0,
 1,0,1,7,400000,5000,25000,
 0,0.2,0,0,'npc_loot_goblin');

-- ---------------------------------------------------------------------------
-- Display model (DisplayID 7107 = "Goblin Mercenary" model). Cosmetic only;
-- swap CreatureDisplayID for any other goblin model you prefer.
-- ---------------------------------------------------------------------------
DELETE FROM `creature_template_model` WHERE `CreatureID` = 400000;
INSERT INTO `creature_template_model`
(`CreatureID`,`Idx`,`CreatureDisplayID`,`DisplayScale`,`Probability`) VALUES
(400000,0,7107,1,1);

-- ---------------------------------------------------------------------------
-- Loot. Gold is handled by mingold/maxgold above; these item rows are just
-- examples - replace them with whatever the goblin should drop.
-- ---------------------------------------------------------------------------
DELETE FROM `creature_loot_template` WHERE `Entry` = 400000;
INSERT INTO `creature_loot_template`
(`Entry`,`Item`,`Reference`,`Chance`,`QuestRequired`,`LootMode`,`GroupId`,
 `MinCount`,`MaxCount`,`Comment`) VALUES
(400000,33470,0,60,0,1,0,1,5,'Loot Goblin - Frostweave Cloth (example)'),
(400000,36912,0,40,0,1,0,1,3,'Loot Goblin - Saronite Ore (example)'),
(400000,49426,0,5,0,1,0,1,1,'Loot Goblin - Emblem of Frost (example)');

-- ---------------------------------------------------------------------------
-- Bind the despawn AuraScript to the spawn debuff (spell 700001).
-- ---------------------------------------------------------------------------
DELETE FROM `spell_script_names` WHERE `spell_id` = 700001
    AND `ScriptName` = 'spell_loot_goblin_despawn';
INSERT INTO `spell_script_names` (`spell_id`,`ScriptName`) VALUES
(700001,'spell_loot_goblin_despawn');
