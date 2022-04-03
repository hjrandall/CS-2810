Recommend
---------

This is the final step of this project where you bring everything
together and recommend the top 20 words to guess according to the
scoring metric developed earlier.

In `recommend.c` write the function `recommend` to match the
prototype given in `wordle.h`. I have also provided a `struct`
definition to help with the process.

The basic approach is this:

*   Scan the entire word list, ignoring non-viable words (as
    identified by `is_viable_candidate`).

*   For each viable word, compute the score. Store both of these
    values in a `struct recommendation` instance (as defined in the
    file).

*   Track up to the top 20 (use the constant `RECOMMENDATION_COUNT`
    as defined in `wordle.h` rather than hard-coding the value 20)
    words.

*   Print the top recommendations with their scores.

Since most of the work is done with helper functions, the main
challenge in this step is tracking the top 20 results. Here is one
approach:

*   Declare an array of `struct recommendation` objects with size
    `RECOMMENDATION_COUNT` + 1. This can be a local variable so you
    do not need to use `malloc` and `free` for this part. Track the
    number of elements used in the array using a regular integer
    that starts with zero.

*   Every time you find a new viable word and compute its score, add
    it to the end of the list. Even if you already have 20 entries
    in the list, there will be room because you initialized the
    array to include one extra space.

*   Starting with the entry you just added, compare it with the
    previous element and see if they are out of order with respect
    to each other. If so, swap them and repeat. You will end up
    swapping the new element into its proper place. The list always
    starts out sorted, and every time you add a new element you swap
    it into its correct place so the list is sorted again. You only
    have to worry about the single new element.

*   If the list was not full before, increment the count. If it was
    full already, leave the count alone and the last slot will be
    re-used. It acts as a temporary overflow entry so that new items
    can be placed in the list and then swapped into place.

To compare two elements, the highest score should always come first.
If two words have the same score, use the function `strcmp` to
compare the strings and put them in alphabetical order as a tie
breaker. Use `man strcmp` for details about how to use it.
