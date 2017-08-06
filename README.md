## Plank
**Line** with active area above and below. Line snaps to the highest blob point inside active area.
*Advantages*:
Fast computation. Plank almost always rises when hand is raised (slow moving hand might not be picked up by blob detection, since motion detection blobs would be too small and considered as a noise).
*Disadvantages:*:
Plank reacts to noise, therefore small blobs must be filtered out.
Plank might snap to head movement, when raised arm is stationary.
