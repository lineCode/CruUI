# Layout Specifications

This document is the specification of layout system.

## rules

### about `width` and `height` in `LayoutParams`
There is three mode in measure: `MatchParent`, `WrapContent`, `Exactly`.

`Exactly` mode is with a specific length, which means it should be of exact size.

`WrapContent` means the control has the size that contains the children exactly.

`MatchParent` means the control has the same measure pattern of parent. If parent is `WrapContent`, then it wraps content, which means both itself and parent will wrap content and if parent has only one control, then they are of the same size. If parent is `Exactly`, then it will be also `Exactly` with the same length. If parent is `MatchParent`, then it will be the same of parent's parent.

### about `max_width`, `max_height`, `min_width`, `min_height`

`max*` specify the max length of a dimension and `min*` specify the min length of a dimension. They are of higher priority of `width` and `height`. Calculated size should be adjusted according to the four properties.
