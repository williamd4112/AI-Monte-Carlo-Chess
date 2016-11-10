#ifndef _EXPANSION_H_
#define _EXPANSION_H_

#include <vector>
#include <iostream>
#include <cassert>
#include <cstdint>

#include "debug.h"
#include "tss.h"
#include "state.h"
#include "constants.h"

namespace mcts
{
void find_all_threats_at_state(const State & state, std::vector<std::vector<Tss::threat_t>> & threats)
{
  assert(threats.size() >= 5);

  Tss tss(state);
  for (int level = THREAT_LEVEL_5; level >= THREAT_LEVEL_2; level--) {
    tss.find_all_threats(attacks[level], level, level);

    /* Early termination when higher level attack found */
    if (!threats[level].empty()) {
      break;
    }
  }
}

void find_best_move(
    const std::vector<std::vector<Tss::threat_t>> & attacks,
    const std::vector<std::vector<Tss::threat_t>> & defends,
    std::vector<Tss::point_t> & moves)
{
  assert(attacks.size() >= 5 && defends.size() >= 5);

  /* Choose highest attack */
  int attack_index = 0;
  for (unsigned int i = 0; i < attacks.size(); i++) {
    if (!attacks[i].empty()) {
      attack_index = i;
      break;
    }
  }

  /* Choose highest defend */
  int defend_index = 0;
  for (unsigned int i = 0; i < defends.size(); i++) {
    if (!defends[i].empty()) {
      defend_index = i;
      break;
    }
  }

  /* No higher, merge or choose double threat */
  if (attack_index < defend_index) {
    /* Choose double threat */
    for (Tss::threat_t threat : attacks[attack_index]) {
      if (threat.gain >= 2) {
        moves.push_back(threat.point);
      }
    }

    /* Or merge level by level, if empty */
    if (moves.empty()) {

    }

  }
  /* Higher, defend */
  else {
    for (Tss::threat_t threat : defends[defend_index]) {
      moves.push_back(threat.point);
    }
  }
}

void expand_state(const State & state, std::vector<State> & expanded_states)
{
  /* Find attack moves */
  std::vector<std::vector<Tss::threat_t>> attacks(5, std::vector<Tss::threat_t>);
  find_all_threats_at_state(state, attacks);

  /* Find defend moves */
  std::vector<std::vector<Tss::threat_t>> defends(5, std::vector<Tss::threat_t>);
  find_all_threats_at_state(state, defends);

  /* Check no attack, defend moves (randomly choose) */
  bool has_threat = false;
  for (int i = 0; i < 5 && !threat; i++) {
    has_threat = !attacks[i].empty() || !defends[i].empty();
  }

  if (has_threat) {

  }
  else {

  }
}
}

#endif
