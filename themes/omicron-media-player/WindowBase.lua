
Alignment = {
    AlignLeft = 0x0001,
    AlignLeading = AlignLeft,
    AlignRight = 0x0002,
    AlignTrailing = AlignRight,
    AlignHCenter = 0x0004,
    AlignJustify = 0x0008,
    AlignAbsolute = 0x0010,
    AlignHorizontal_Mask = 31,
    AlignTop = 0x0020,
    AlignBottom = 0x0040,
    AlignVCenter = 0x0080,
    AlignBaseline = 0x0100,
    AlignVertical_Mask = 480,
    AlignCenter = 132
}

Orientation = {
    Horizontal = 0x1,
    Vertical = 0x2
}

Direction = {
    LeftToRight = 0,
    RightToLeft = 1,
    TopToBottom = 2,
    BottomToTop = 3,
    Down = TopToBottom,
    Up = BottomToTop
}

BoxLayout.Horizontal = Direction.LeftToRight
BoxLayout.Vertical = Direction.TopToBottom
