cc.Class({
    extends: cc.Component,

    properties: {
        numLabel: {
            default: null,
            type: cc.Label,
        },
        number: {
            get() {
                return this._number ? this._number: 0;
            },
            set(value) {
                this._number = value;
                this.numLabel.string = this._number;
                let color = this.getColor(value);
                this.node.color = color;
            },
        },
        target: {
            get() {
                return this._target ? this._target: 0;        
            },
            set(value) {
                this._target = value;
            },
        },
    },

    // LIFE-CYCLE CALLBACKS:

    onLoad() {
        this.numLabel.enabled = false;
    },

    start() {
    },
    init() {
        this.numLabel.enabled = true;
        let rand = Math.floor(Math.random() * 10);
        if (rand < 8) 
        {
            this.number = 2;
        } else {
            this.number = 4;
        }
    },
    getColor(number) {
        switch(number)
        {
            case 2:
                return cc.color(255, 87, 51);
            case 4:
                return cc.color(241, 186, 27);
            case 8:
                return cc.color(192, 236, 10);
            case 16:
                return cc.color(65, 241, 4);
            case 32:
                return cc.color(17, 232, 150);
            case 64:
                return cc.color(44, 149, 242);
            case 128:
                return cc.color(255, 87, 51);
            case 256:
                return cc.color(19, 12, 218);
            case 512:
                return cc.color(159, 22, 232);
            case 1024:
                return cc.color(241, 52, 144);
            case 2048:
                return cc.color(235, 18, 18);
            default:
                return cc.color(255, 240, 1);
        }   

        return cc.color(255, 240, 1);        
    },
    compose()
    {
        if (this._number) {
            this.number = this._number * 2;
        }
        this.node.runAction(cc.sequence(
            cc.scaleTo(0.1, 1.1),
            cc.scaleTo(0.1, 1),
        ));
    }
});
