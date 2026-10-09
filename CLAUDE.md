# mod-loot-goblins

A Diablo-style **Loot Goblin** for Forgotten Land: when a player kills a creature, a fast, harmless goblin
may appear at the corpse and flee for 10 seconds. Killed in time, it either drops gold and items or splits
into two fresh goblins, which may split again (the code sets no depth limit; only goblins that do not
split are lootable). The spawn chance
depends on the victim's rank and the map type; two stackable GM-granted auras raise the spawn and split
chances. Full description: [README.md](README.md).

## Ids and tables

| What | Id / name |
|---|---|
| Creature | **400000** "Loot Goblin" `<Treasure Hoarder>`: level 80, faction 7, display 7107, `ScriptName` `npc_loot_goblin`, loot id 400000, gold 5,000-25,000 copper |
| Spells (server only, `spell_dbc`) | **700001** Loot Goblin Despawn Timer (the goblin's 10 s debuff) · **700002** Loot Goblin Fortune (spawn talent, 6 stacks) · **700003** Loot Goblin Greed (split talent, 6 stacks) |
| Spell script | `spell_loot_goblin_despawn` (AuraScript, bound to 700001 in `spell_script_names`) |
| C++ scripts | `npc_loot_goblin` (CreatureScript), `LootGoblins_KillScript` (PlayerScript), `LootGoblins_ConfigWorldScript` (WorldScript) |
| World rows | `creature_template` + `creature_template_model` 400000, `creature_loot_template` 400000 (Frostweave Cloth 33470, Saronite Ore 36912, Emblem of Frost 49426: the README's example loot), `spell_script_names` 700001, `spell_dbc` 700001-700003 |
| Own tables | none |
| Config | `loot_goblins.conf`, `LootGoblins.*` |

None of these ids is in the vault registry `06-custom-ids.md`. Measured 2026-10-09: the server `Spell.dbc`
file has no row in 700000-700999, and the workbench's `creature_template` 400000 is this module's row; CoA's
spell import uses none of 700001-700003 (vault `coa-classes/program-records/m1a-report.md` §7).

## Status and progress

- Where it runs: workbench, built from `azerothcore-wotlk/modules/mod-loot-goblins` (`main` `ee24d93`); the
  world SQL was applied 2026-07-12 and the live `loot_goblins.conf` equals the `.dist` (`LootGoblins.Enable
  = 1`). Host: `ee24d93`, older than the MIG ledger (in the 25-module baseline of
  `forgotten-land/15-host-migration-log.md` §3; host conf = the `.dist` values, nightly backup copy
  2026-10-09). Players on both meet goblins at the default chances. Nothing in FL grants the two talent auras
  (world DB, deployed Lua and the other modules searched 2026-10-09).
- Evidence: **T2 for the loot path only**: in the operator's core-sync check of 2026-09-19 (row 8, vault
  `forgotten-land/18-core-upstream-sync-084c9e2.md` §9) the corpse of a `.npc add 400000` goblin was
  lootable. The spawn roll on kills, the split and the talents have no recorded in-game check.
- Done:
  - Spawn roll on every player kill by victim rank (normal / elite / boss) and map type (open world /
    dungeon / raid; none in battlegrounds and arenas), multiplied by the killer's spawn talent.
  - The goblin: passive, fast, pathfinding-aware wandering; a 10 s debuff whose AuraScript despawns only a
    living goblin, so a killed one keeps its lootable corpse.
  - Split on death (two children, the splitting corpse drops nothing); config cached at load.

## Next steps

1. List `EquippedItemClass = -1` in `01_loot_goblins_spell_dbc.sql` (todo, medium; vault trap
   `ai-notes/spell-dbc-equippeditemclass-trap.md`), with a MIG entry for the host.
2. An in-game or bot check of the spawn roll, the split chain and the talents (todo, medium).
3. (suggestion) Replace the README's example loot with FL loot, decided by the operator (todo, low).

Open points in full: [todo.md](todo.md).

## Working here

- Branch `claude/<topic>-<sessionId>`, merge into `main` and push (project rule: no pull requests).
- The two world SQL files are DELETE + INSERT; the DB updater re-applies a file whose bytes change. Every
  `spell_dbc` INSERT lists `EquippedItemClass, EquippedItemSubclass, EquippedItemInvTypes` as `-1, 0, 0`.
- Test a goblin with `.npc add 400000` (not `.npc add temp`, whose corpse vanishes in the tick it dies,
  vault FL/18 §9), kill it within 10 s, then `.npc delete` the spawn; `.aura 700002` / `.aura 700003` add one
  talent stack per call. Raise `LootGoblins.Chance.*` on the workbench to test the spawn roll. Test bots and
  the client probe use their own TBOT / CRTEST accounts only.
- Build tree `C:\wowstuff\dcore_bin`; restart the workbench only with the workspace's
  `scripts\worldserver_restart.ps1` under `tools\shared.lock` (vault `13-bug-report-playbook.md` §3).
- Host-relevant changes (code, SQL, conf) need a MIG entry in share-public
  `docs/World of Warcraft/forgotten-land/15-host-migration-log.md`.
- Vault (`share-public/docs/World of Warcraft/`): `forgotten-land/18-core-upstream-sync-084c9e2.md` §9,
  `forgotten-land/15-host-migration-log.md` §3, `forgotten-land/10-mass-pull-performance-plan.md`,
  `06-custom-ids.md`, `ai-notes/spell-dbc-equippeditemclass-trap.md`.
- Doc set: INDEX.md, CLAUDE.md, data_structure.md, functions.md, log.md (newest first), todo.md.
