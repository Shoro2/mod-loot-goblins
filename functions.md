# Functions

## Scripts

| Script | Type and hook | What |
|---|---|---|
| `LootGoblins_ConfigWorldScript` | `WorldScript`, `void OnAfterConfigLoad(bool reload)` (`WORLDHOOK_ON_AFTER_CONFIG_LOAD`) | copies every `LootGoblins.*` key into `gLootGoblinsCfg`; the struct's in-code defaults cover a kill before the first load |
| `LootGoblins_KillScript` | `PlayerScript`, `void OnPlayerCreatureKill(Player* killer, Creature* killed)` (`PLAYERHOOK_ON_CREATURE_KILL`) | the spawn roll |
| `npc_loot_goblin` | `CreatureScript`, AI `npc_loot_goblinAI : ScriptedAI` | the goblin's behaviour and split |
| `spell_loot_goblin_despawn` | `AuraScript` on 700001, `OnEffectRemove` (EFFECT_0, `SPELL_AURA_DUMMY`, `AURA_EFFECT_HANDLE_REAL`) | despawn on expiry |

Loader: `Addmod_loot_goblinsScripts()` registers the config script, then `AddSC_loot_goblin_npc()`,
`AddSC_loot_goblin_world()`, `AddSC_loot_goblin_spell()`.

## Spawn roll (`OnPlayerCreatureKill`)

1. Return when disabled, when killer or victim is missing, or when the victim is a goblin itself (its own
   death is handled by the split).
2. Rank: boss = `IsDungeonBoss() || isWorldBoss()`, else elite = `isElite()`, else normal.
3. Map: not `Instanceable()` -> `Chance.World.*`; `IsRaid()` -> `Chance.Raid.*`; `IsDungeon()` ->
   `Chance.Dungeon.*`; anything else (battlegrounds, arenas) -> no spawn.
4. Spawn talent: `stacks = min(GetAuraCount(SpawnTalent.SpellId), SpawnTalent.MaxStacks)`;
   `chance *= 1 + BonusPerStack / 100 * stacks` (x4 at six stacks with the defaults).
5. `roll_chance_f(chance)` (percent); on success the killer summons `GoblinEntry` at the victim's position,
   `TEMPSUMMON_CORPSE_TIMED_DESPAWN` with `CorpseDespawnMs`.

## The goblin (`npc_loot_goblinAI`)

- `InitializeAI`: `REACT_PASSIVE`, run mode, walk and run speed set to `MoveSpeedRate`,
  `MoveRandom(WanderRadius)` (the random movement generator uses the mmaps, so no running through walls),
  then `AddAura(700001)` with its duration forced to `DebuffDurationMs` (the DBC duration does not matter).
- `AttackStart`, `MoveInLineOfSight` and `UpdateAI` are empty: it never engages. Faction 7 keeps it
  attackable.
- `JustDied(killer)`: split chance = `SplitChance` + `SplitTalent.BonusPerStack` x `min(stacks, MaxStacks)`
  of the killer's controlling player (`GetCharmerOrOwnerPlayerOrPlayerItself`; 5 % + 5 x 6 = 35 % at six
  stacks). On a split it summons two goblins 1-3 yd away (`MovePositionToFirstCollision`, same summon type),
  clears its loot and removes `UNIT_DYNFLAG_LOOTABLE`: a goblin that splits drops nothing. Children can
  split again; the code sets no depth limit (the README's "full chain 1 -> 2 -> 4" is an example).

## Despawn aura (`spell_loot_goblin_despawn`)

On removal of 700001 with `AURA_REMOVE_BY_EXPIRE` and a living target creature: `DespawnOrUnsummon()`.
Death removes the aura with another mode, so the lootable corpse stays for `CorpseDespawnMs`.

## Talents (700002 / 700003)

Plain stacking dummy auras that only the server reads (`GetAuraCount`); they live in `spell_dbc` alone, so
the client shows no buff icon. The module never grants them: `.aura 700002` / `.aura 700003` adds one stack
per call (`CumulativeAura` 6). Their `spell_dbc` rows lack `EquippedItemClass = -1` (see todo.md).

## Traps

- `.npc add temp 400000` cannot test the loot: that summon type removes the corpse in the tick it dies.
  Use `.npc add 400000` and delete the spawn afterwards (vault `forgotten-land/18-core-upstream-sync-084c9e2.md` §9).
- The goblin is level 80 everywhere, whatever the victim's level.

## Config

Keys and defaults: [data_structure.md](data_structure.md).
