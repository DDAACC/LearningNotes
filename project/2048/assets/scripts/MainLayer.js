cc.Class({
    extends: cc.Component,

    properties: {
        gridPrefab: {
            default: null,
            type: cc.Prefab,
        },
        scoreLabel: {
            default: null,
            type: cc.Label,
        },
        _blocks: [],
        _score: 0,
    },

    // LIFE-CYCLE CALLBACKS:

    onLoad () {
        cc.systemEvent.on(cc.SystemEvent.EventType.KEY_UP, this.onKeyUp, this);
    },

    start() {
        this.initBg();
        this.gameStart();
    },
    initBg() {
        for(let x = 1; x <= 4; ++x) {
            for(let y = 1; y <= 4; ++y) {
                let gridNode = cc.instantiate(this.gridPrefab);
                gridNode.parent = this.node;
                let posX = this.gridToPos(x);
                let posY = this.gridToPos(y);
                gridNode.setPosition(posX, posY);
            }
        }
    },
    gameStart() {   
        this.updateScore(0);

        for(let x = 1; x <= 4; ++x) {
            for(let y = 1; y <= 4; ++y) {
                let index = this.gridToIndex(x, y)
                let blockNode = this._blocks[index];
                if(blockNode) {
                    blockNode.destroy();
                }
                this._blocks[index] = false;
            }
        }

        this.getNewBlock();
    },
    gameEnd() {
        console.log("你死了");
    },
    updateScore(add) {
        add = add || 0;
        this._score = add > 0 ? this._score + add: 0;
        this.scoreLabel.string = this._score;
    },
    onKeyUp () {
        switch(event.keyCode) {
            case cc.macro.KEY.left:
                this.move(1);
                break;
            case cc.macro.KEY.right:
                this.move(2);
                break;
            case cc.macro.KEY.up:
                this.move(3);
                break;
            case cc.macro.KEY.down:
                this.move(4);
                break;
        }
    },
    move(direction) {
        let self = this;
        let canMove = false;
        let hasGet = false;
        let isIncrease = direction === 1 || direction === 4;
        let isTransverse = direction === 1 || direction === 2;
        for(let outerIndex = 1; outerIndex <= 4; ++outerIndex) {
            let idleIndex = [];
            let lastNode = null;
            let innerIndex = isIncrease ? 1 : 4;
            for(; innerIndex >= 1 && innerIndex <= 4;) {             
                let x = isTransverse ? innerIndex : outerIndex;
                let y = isTransverse ? outerIndex : innerIndex;
                let index = this.gridToIndex(x, y);
                let blockNode = this._blocks[index];
                if(blockNode === false) {
                    idleIndex.push(innerIndex);
                } else {
                    let target = idleIndex[0] || innerIndex;
                    let block = blockNode.getComponent("Block");
                    let lastBlock = lastNode && lastNode.getComponent("Block");
                    let number = block.number;
                    let compose = false;
                    if (lastBlock && number === lastBlock.number) {
                        target = lastBlock.target;
                        lastNode = null;
                        compose = true;
                    } else {
                        lastNode = blockNode;
                        idleIndex.shift();
                    }
                    block.target = target;
                    let delta = innerIndex - target;
                    if(delta != 0) {
                        canMove = true;
                        this._blocks[index] = false;
                        idleIndex.push(innerIndex);
                        if(!compose) {
                            let newPosX = isTransverse ? target : x;
                            let newPosY = isTransverse ? y: target;
                            let newIndex = this.gridToIndex(newPosX, newPosY);
                            this._blocks[newIndex] = blockNode;
                        }
                        let deltaX = isTransverse ? - delta * 100 : 0;
                        let deltaY = isTransverse ? 0: - delta * 100;
                        blockNode.runAction(cc.sequence(
                            cc.moveBy(0.05, deltaX, deltaY),
                            cc.callFunc(() => {
                                if(compose) {
                                    lastBlock.compose();
                                    blockNode.destroy();
                                    self.updateScore(number * 2);                                        
                                } 

                                if(canMove && !hasGet) {
                                    hasGet = true;
                                    this.getNewBlock();
                                }
                            })
                        ));
                    }
                }
                isIncrease ? ++innerIndex : --innerIndex;
            }
        }
    },
    gridToIndex(gridX, gridY) {
        return (gridX - 1) * 4 + gridY;
    },
    gridToPos(grid) {
        return (grid - 2) * 100 - 50;
    },
    indexToGrid(index) {
        let gridX = Math.floor((index - 1) / 4) + 1;
        let gridY = index - gridX * 4 + 4;
        return {x:gridX, y:gridY};
    },
    getNewBlock() {
        let index = this.getIdleIndex();
        if(index === -1) {
            this.gameEnd();
            return
        }

        let blockNode = cc.instantiate(this.gridPrefab);
        blockNode.parent = this.node;
        let block = blockNode.getComponent("Block");
        block.init();

        this._blocks[index] = blockNode;
        
        let grid = this.indexToGrid(index);
        let posX = this.gridToPos(grid.x);
        let posY = this.gridToPos(grid.y);
        blockNode.setPosition(posX, posY);
    },

    getIdleIndex() {
        let idles = [];
        for(let index in this._blocks) {
            if(this._blocks[index] === false) {
                idles.push(index);
            }
        }

        if(idles.length > 0) {
            let rand = Math.floor(Math.random() * idles.length);
            return idles[rand];
        }

        return -1;
    },
    // update (dt) {},
});
