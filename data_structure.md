# Data structure

| Path | What |
|---|---|
| `README.md` | features, install, config highlights, talents, custom ids |
| `conf/loot_goblins.conf.dist` | the `LootGoblins.*` keys (installed as `loot_goblins.conf`) |
| `src/LootGoblins.h` | `SPELL_LOOT_GOBLIN_DESPAWN = 700001`, `struct LootGoblinsConfig` (all tunables with in-code defaults), `extern gLootGoblinsCfg` |
| `src/loot_goblins_loader.cpp` | `gLootGoblinsCfg`, `LootGoblins_ConfigWorldScript`, `Addmod_loot_goblinsScripts()` |
| `src/LootGoblin_WorldScript.cpp` | `LootGoblins_KillScript` (the spawn roll on kills) |
| `src/LootGoblin_NPC.cpp` | `npc_loot_goblin` and its AI (wander, debuff, split) |
| `src/LootGoblin_SpellScript.cpp` | `spell_loot_goblin_despawn` (AuraScript) |
| `data/sql/db-world/base/00_loot_goblins_world.sql` | creature 400000, its model and loot, the `spell_script_names` row |
| `data/sql/db-world/base/01_loot_goblins_spell_dbc.sql` | `spell_dbc` 700001-700003 |

## World rows (written by the SQL, DELETE + INSERT)

| Table | Rows |
|---|---|
| `creature_template` | 400000 "Loot Goblin" / "Treasure Hoarder", level 80, exp 2, faction 7, humanoid, `HealthModifier` 0.2, `RegenHealth` 0, `mingold` 5000 / `maxgold` 25000, `MovementType` 0, `ScriptName` `npc_loot_goblin` |
| `creature_template_model` | 400000, idx 0, display 7107 |
| `creature_loot_template` | 400000: 33470 (60 %, 1-5), 36912 (40 %, 1-3), 49426 (5 %, 1), all marked "example" |
| `spell_script_names` | 700001 `spell_loot_goblin_despawn` |
| `spell_dbc` | 700001 (`CumulativeAura` 0), 700002 and 700003 (`CumulativeAura` 6); apply-aura + `SPELL_AURA_DUMMY` on the caster, `DurationIndex` 21 (infinite); `EquippedItemClass` not listed (table default 0, see todo) |

No own tables. Character state lives only in the auras (700002 / 700003 stacks on the player).

## Config keys (`loot_goblins.conf`, read in `OnAfterConfigLoad`)

| Key | Default | Meaning |
|---|---|---|
| `LootGoblins.Enable` | 1 | master switch |
| `LootGoblins.GoblinEntry` | 400000 | creature entry of the goblin (must match the SQL) |
| `LootGoblins.DebuffDurationMs` | 10000 | lifetime of a living goblin |
| `LootGoblins.CorpseDespawnMs` | 30000 | how long a corpse stays lootable |
| `LootGoblins.MoveSpeedRate` | 2.5 | walk and run speed multiplier |
| `LootGoblins.WanderRadius` | 8.0 | wander radius in yards |
| `LootGoblins.SplitChance` | 5.0 | base split chance, percent |
| `LootGoblins.SplitTalent.SpellId` / `.BonusPerStack` / `.MaxStacks` | 700003 / 5.0 / 6 | flat percentage points per stack |
| `LootGoblins.SpawnTalent.SpellId` / `.BonusPerStack` / `.MaxStacks` | 700002 / 50.0 / 6 | percent multiplier per stack |
| `LootGoblins.Chance.World.Normal` / `.Elite` / `.Boss` | 0.01 / 0.05 / 1.0 | open world, percent |
| `LootGoblins.Chance.Dungeon.Normal` / `.Elite` / `.Boss` | 0.05 / 0.1 / 0.5 | 5-player dungeons |
| `LootGoblins.Chance.Raid.Normal` / `.Elite` / `.Boss` | 0.1 / 0.2 / 1.0 | raids |

Deployed (2026-10-09): workbench `C:\wowstuff\dcore\configs\modules\loot_goblins.conf` and the host copy
(nightly backup) carry exactly these values.
