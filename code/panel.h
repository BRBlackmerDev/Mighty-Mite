
struct ACTIVE_RECT {
    rect  Bound;
    boo32 isHover;
    union {
        boo32 IsActive;
        boo32 isSelected;
    };
};

struct CLICK_RECT {
    rect  Bound;
    boo32 isHover;
};