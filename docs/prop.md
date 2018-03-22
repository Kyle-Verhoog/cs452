% CS 452 Project Proposal: Space Wyrm
% Benjamin Zhao, Kyle Verhoog

## Overview
- You should see locomotives playing a version of laser tag
- Locomotives are "stunned" (slow down, become uncontrollable) when shot
- The shooter locomotive can choose to absorb their stunned victims to form a train
- Trains are broken apart by being shot
- To make the game non-trivial, a switch is toggled once a train has crossed it
- The goal of the game is to absorb all the locomotives on the track
- The game devolves to "chicken" when two trains are on a head-on-collision course

## Technical Challenges
1. "Absorbing" the trains
- This requires knowing fairly precise train positions and speeds as well as
  accounting for trains travelling at different speeds with the same settings.
2. Knowing position and timing well enough to toggle switches as soon as
   a train has crossed the switch.
- This requires the ability to control or manage the ordering of commands
  buffered to the train controller as well as accurately knowing position and
  speed of the train.
3. Ability to drive trains with more than one locomotive precisely
- This requires being able to modulate a fairly similar speed for several
  locomotives in a train
4. Being able to manage potential collisions gracefully (avoid them)

## Technical Solutions

1. Given 2. we have all of the data we need to calculate and adjust speeds and
   distances at which the trains can gracefully connect. Since our models are
   accurate we can confidently determine the distance between the trains and
   the corresponding speeds to have them connect at a point of our choosing.

2. We can generate models for acceleration and velocity more precisely by using
   the method: ("Doing Better" section of lecture 19). We can also record more
   precise measurements by external means (using phone camera with tape
   measure). Knowing the positions and speeds accurately we know when the train
   has finished crossing over the switch.

3. We can create continuous models of speed for each locomotive in order to
   find the points in time in which we need to send either a speed up or speed
   down command. We can fit a polynomial equation to approximate the speed
   between settings. This allows us to keep groups of locomotives moving at a
   similar speed.

4. Precompute future track states using our prediction model in order to
   prevent collisions that may happen in the future. By having these
   precomputed outcomes we can evaluate very quickly and accomodate for
   a potential collision.
