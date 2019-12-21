# Hex-Rom
For generating hex ROM for redstone in minecraft.

This program brute force searches all possible permuations for the given settings to find rom hex values that produce the desired sequence(s).

The main goal of this program is for optimizing displays, currently for a 7 segment display and perhaps other displays. It can also in theory be used for any hex logic. (There are plans to do this).

The key to the hex ROM is that this type of hex rom can compress data to the O(n) space complexity where n is the number of changes in a sequence. A change being defined as a change in direction in the desired sequence. 00123210 has two changes in direction for example. 101100 has three. Compared to regular rom where it scales to the lenght of the output sequence. This allows theoretical huge compression of data should the data have enough of the right patterns to it.(it has worked in practice for 7 segs)

For now I see no need for more explanation as this is all in early stages.

-Aminotreal
