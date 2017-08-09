## Plank
**Line** with active area above and below. Line snaps to the highest blob point inside active area.

**Advantages**:  
Fast computation. Plank almost always rises when hand is raised (slow moving hand might not be picked up by blob detection, since motion detection blobs would be too small and considered as a noise).

**Disadvantages**:  
Plank reacts to noise, therefore small blobs must be filtered out.
Plank might snap to head movement, when raised arm is stationary.

# TO DO
Detect when hand is rising. (Significant blob above head level).

### Current flaws of blob detection
Small blobs inside big ones.
Palm is frequently not selected. Especially at the end of the movement, when motion is minimal.

## Hand detection ideas
Setup blob detection, so the palm is always present in a blob.
Use higher vertical min_distance than horizontal.

IDEA!! Palm tracking. When movement is clear (the whole hand in motion, big connected blob), select the top blob area, that fits into a square. Square dimensions are either fixed or dynamically determined by the hand blob size. After that use only motion inside this square and some active area around it (think plank in 2d) to track the palm location.

Palm area center can snap to the weight center inside of it.
When hand is lowered, discard the palm and start again.


# NEW PLAN
We don't need palm position, neither hand position. Just find out if the hand has been raised or not.
Figure that out by analysing the motion "disturbance" direction (imagine hand motion as a wave). If the direction is upwards then the hand is raised, if its downwards -- hand is lowered.
## Blob Benchmark notes
Pay close attention to 129, 137, 156, 180
