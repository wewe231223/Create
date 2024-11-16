struct BilllBoard_VS_IN
{
    float3 position         : POSITION;
    uint halfWidth          : WIDTH;
    uint height             : HEIGHT;
    float3 up               : UP; 
    bool spritable          : SPRITABLE;
    uint spriteFrameInRow   : SPRITEFRAMEINROW;
    uint spriteFrameInCol   : SPRITEFRAMEINCOLUMN;
    uint spriteUnitWidth    : SPRITEUNITWIDTH;
    uint spriteUnitHeight   : SPRITEUNITHEIGHT;
    float spriteDuration    : SPRITEDURATION;
};


