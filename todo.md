# Todo

- (medium) `data/sql/db-world/base/01_loot_goblins_spell_dbc.sql` does not list `EquippedItemClass`, so
  700001-700003 get the table default 0 = "requires a consumable" (measured on the workbench 2026-10-09;
  vault trap `ai-notes/spell-dbc-equippeditemclass-trap.md`). By code reading, a talent stack a player put
  on itself (`.aura 700002`) is removed at the next equipment change: `Player::RemoveItemDependentAurasAndCasts`
  asks `HasItemFitToSpellRequirements`, which logs "Not handled spell requirement for item class 0" and
  answers false. Not reproduced. Fix: list `EquippedItemClass, EquippedItemSubclass, EquippedItemInvTypes`
  as `-1, 0, 0`; the updater re-applies the file (MIG entry for the host).
- (medium) No in-game or bot check yet of the spawn roll on kills, the split chain and the two talents; only
  the loot of a `.npc add 400000` corpse is T2 (vault `forgotten-land/18-core-upstream-sync-084c9e2.md` §9,
  row 8).
- (low) The loot is still the README's example (Frostweave Cloth 60 %, Saronite Ore 40 %, Emblem of Frost
  5 %, 5,000-25,000 copper); FL's own goblin loot is the operator's call.
- (low) Register creature 400000 and spells 700001-700003 in the vault's `06-custom-ids.md`.
