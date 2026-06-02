# mod-loot-goblins

An AzerothCore module that adds a Diablo-style **Loot Goblin**: a fast, harmless
critter that occasionally appears when you kill a creature, runs around for a
few seconds, can split into more goblins when killed, and drops loot.

## Features

- **Loot Goblin NPC** that never attacks, wanders quickly and erratically
  (pathfinding/mmap aware, so it never runs into walls), and carries a spawn
  debuff that despawns it after 10 seconds if it isn't killed first.
- **Random spawns on kills.** When a player kills a creature there is a chance a
  Loot Goblin spawns at the corpse. The chance depends on the victim's rank
  (normal / elite / boss) and the map type (open world / dungeon / raid).
- **Splitting.** A killed goblin has a chance to split into two fresh goblins
  instead of dropping loot. The children can split again, so a full chain is
  `1 -> 2 -> 4` and only the non-splitting "leaves" are lootable (4, not 7).
- **Two stackable talents** (admin-granted auras, max 6 stacks each):
  - **Spawn talent** – multiplies the spawn chance: `chance * (1 + 0.5 * stacks)`
    (×4 at 6 stacks).
  - **Split talent** – adds flat percentage points to the split chance:
    `5% + 5 * stacks` (35% at 6 stacks).
- **Despawn handled by a spell script.** When the spawn debuff expires the
  living goblin despawns; if it was killed, the debuff is removed by death and
  the lootable corpse is left alone.
- Fully configurable (chances, durations, speed, split %, talent IDs/bonuses).

## Requirements

A working AzerothCore (this fork) server. The module ships its own custom
spells via the `spell_dbc` world table, so no client `Spell.dbc` edits are
required for the mechanics to work.

## Installation

```bash
# from your AzerothCore source root
cd modules
git clone <this-repo> mod-loot-goblins
cd ..
# rebuild
cd build && cmake .. -DSCRIPTS=static -DMODULES=static && make -j
```

Import the SQL in `data/sql/db-world/base/` into your **world** database
(`acore_world`) — either via the DB updater or manually:

```bash
mysql acore_world < modules/mod-loot-goblins/data/sql/db-world/base/00_loot_goblins_world.sql
mysql acore_world < modules/mod-loot-goblins/data/sql/db-world/base/01_loot_goblins_spell_dbc.sql
```

Copy `conf/loot_goblins.conf.dist` to your worldserver config directory as
`loot_goblins.conf` (or edit in place) and restart the worldserver.

## Configuration

See `conf/loot_goblins.conf.dist` for the full, commented list. Highlights:

| Key | Default | Meaning |
|-----|---------|---------|
| `LootGoblins.Enable` | 1 | Master switch |
| `LootGoblins.GoblinEntry` | 400000 | creature_template entry (must match SQL) |
| `LootGoblins.DebuffDurationMs` | 10000 | Lifetime while alive |
| `LootGoblins.CorpseDespawnMs` | 30000 | Lootable corpse window |
| `LootGoblins.MoveSpeedRate` | 2.5 | Movement speed multiplier |
| `LootGoblins.WanderRadius` | 8.0 | Wander radius (yards) |
| `LootGoblins.SplitChance` | 5.0 | Base split chance (%) |
| `LootGoblins.SplitTalent.*` | 700003 / 5.0 / 6 | Split talent id / +points per stack / max stacks |
| `LootGoblins.SpawnTalent.*` | 700002 / 50.0 / 6 | Spawn talent id / +% per stack / max stacks |
| `LootGoblins.Chance.World.*` | 0.01 / 0.05 / 1.0 | Open world normal / elite / boss (%) |
| `LootGoblins.Chance.Dungeon.*` | 0.05 / 0.1 / 0.5 | Dungeon normal / elite / boss (%) |
| `LootGoblins.Chance.Raid.*` | 0.1 / 0.2 / 1.0 | Raid normal / elite / boss (%) |

Spawn chances default to 1/10 of the "full" tuning, so goblins are rare until
the spawn talent ramps the chance up.

## Talents

Both talents are plain stackable auras the server reads via `GetAuraCount`. Grant
them however you like (a learnable spell, an item, a reward, or a GM command):

```
.aura 700002    # spawn-chance talent  (repeat up to 6x for 6 stacks)
.aura 700003    # split-chance talent  (repeat up to 6x for 6 stacks)
```

Because they live only in `spell_dbc` (server side), the client will not render
a buff icon for them. If you want a visible buff with stacks, add the spell to
your client `Spell.dbc` in the fork's custom ID range and point the
`*.SpellId` config keys at it.

## Custom IDs

| ID | Purpose |
|----|---------|
| creature `400000` | Loot Goblin |
| spell `700001` | Spawn/despawn debuff (NPC) |
| spell `700002` | Spawn-chance talent |
| spell `700003` | Split-chance talent |

The creature display (DisplayID `7107`) and the example loot in
`00_loot_goblins_world.sql` are cosmetic — change them to taste.

## License

GPL v2, matching AzerothCore.
