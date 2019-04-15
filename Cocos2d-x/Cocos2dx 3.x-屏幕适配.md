### Cocos2dx 3.x-屏幕适配

```C++
bool Sprite::initWithPolygon(const cocos2d::PolygonInfo &info)
{
    bool ret = false;

    Texture2D *texture = _director->getTextureCache()->addImage(info.getFilename());
    if(texture && initWithTexture(texture))
    {
        _polyInfo = info;
        _renderMode = RenderMode::POLYGON;
        Node::setContentSize(_polyInfo.getRect().size / _director->getContentScaleFactor());
        ret = true;
    }

    return ret;
}
```

