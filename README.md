# How this * works?
It's simple:
- motion detection
- track peaks (highest motion point) over the last 5 frames in a queue
- peaks strictly ascend? - hand has been raised
- peaks strictly descend? - hand has been lowered

Ok, now it works for one person. For it to work for multiple people, we would need to separate them into individual areas.

Face detection -> face bounding rectangles -> triple (9-tuple?) the size -> remove overlaps -> you got some person bounding areas -> use the aforementioned algorithm for each area.

[![demostration](http://img.youtube.com/vi/LFfiU21jzaU/0.jpg)](https://youtu.be/LFfiU21jzaU)