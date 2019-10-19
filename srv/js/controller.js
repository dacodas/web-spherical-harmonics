
let HarmonicDOMParser = new DOMParser();

function Harmonic(id, l, m, amplitude, phase)
{
    this.id = id;

    this.l = l;
    this.m = m;
    this.amplitude = amplitude;
    this.phase = phase;

    this.source = this.template.replace('%%% NUMBER %%%', this.id);
    this.element = this.elementFromString(this.source);

    // By contract the first one will be the correct one
    this.lTextField = this.element.getElementsByClassName("l-text-field")[0]
    this.mTextField = this.element.getElementsByClassName("m-text-field")[0]
    this.amplitudeTextField = this.element.getElementsByClassName("amplitude-text-field")[0]
    this.lIncrementButton = this.element.getElementsByClassName("l-increment-button")[0]
    this.lDecrementButton = this.element.getElementsByClassName("l-decrement-button")[0]
    this.mIncrementButton = this.element.getElementsByClassName("m-increment-button")[0]
    this.mDecrementButton = this.element.getElementsByClassName("m-decrement-button")[0]
    this.amplitudeSlider = this.element.getElementsByClassName("amplitude-slider")[0]

    this.setL(l)
    this.setM(m)
    this.setAmplitude(amplitude)

    this.addEventListeners()
}

Harmonic.prototype.serialize = function()
{
    return [this.l,
            this.m,
            this.amplitude.toFixed(2),
            this.phase.toFixed(2)].map(l => l.toString()).join(" ")
}

Harmonic.prototype.elementFromString = function (string)
{
    doc = HarmonicDOMParser.parseFromString(string, 'text/html')
    xpath_result = doc.evaluate("/html/body/div", doc, null, XPathResult.FIRST_ORDERED_NODE_TYPE)
    return xpath_result.singleNodeValue
}

Harmonic.prototype.addEventListeners = function ()
{
    var parent = this;

    this.amplitudeSlider.addEventListener(
        'change',
        function(event) {
            parent.setAmplitudeFromSlider(this.value);
        });

    this.lIncrementButton.addEventListener(
        'mouseup',
        function(event) {
            parent.setL(parent.l + 1);
        });

    this.lDecrementButton.addEventListener(
        'mouseup',
        function(event) {
            parent.setL(parent.l - 1);
        });

    this.mIncrementButton.addEventListener(
        'mouseup',
        function(event) {
            parent.setM(parent.m + 1)
        });

    this.mDecrementButton.addEventListener(
        'mouseup',
        function(event) {
            parent.setM(parent.m - 1)
        });
}


Harmonic.prototype.setL = function (value)
{
    if ( value >= this.m && value >= 0 )
    {
        this.l = value;
        this.lTextField.value = this.l
    }
}

Harmonic.prototype.setM = function (value)
{
    if ( value <= this.l && value >= 0 )
    {
        this.m = value;
        this.mTextField.value = this.m
    }
}

Harmonic.prototype.setAmplitude = function (value)
{
    // Let's see if this works...
    value *= 100
    this.amplitudeSlider.value = value;
    this.setAmplitudeFromSlider(value)
}

Harmonic.prototype.setAmplitudeFromSlider = function (value)
{
    this.amplitude = value / 100;
    this.amplitudeTextField.value = "" + this.amplitude.toFixed(2);
}

Harmonic.prototype.template = `<div class="harmonic-adjuster" id="harmonic-adjuster-%%% NUMBER %%%">
                <table>
                    <tr>
                        <td>
                            <span>ell</span>
                        </td>
                        <td>
                            <input type="text" class="l-text-field" value="5" readonly>
                        </td>
                        <td>
                            <button class="l-decrement-button">\u2212</button>
                            <button class="l-increment-button">+</button>
                        </td>
                    </tr>
                    <tr>
                        <td>
                            <span>em</span>
                        </td>
                        <td>
                            <input type="text" class="m-text-field" value="5" readonly>
                        </td>
                        <td>
                            <button class="m-decrement-button">\u2212</button>
                            <button class="m-increment-button">+</button>
                        </td>
                    </tr>
                    <tr>
                        <td>Amplitude</td>
                        <td>
                            <input type="text" class="amplitude-text-field" value="0.8" readonly>
                        </td>
                        <td>
                            <input type="range" class="amplitude-slider" min="0" max="100" value="80">
                        </td>
                    </tr>
                </table>
            </div>`

Harmonic.prototype.changeAmplitude = function ()
{

};

function Harmonics()
{
    this.harmonicsCount = 0
    this.harmonics = []
    this.addHarmonic(3, 0, 1.0, 0.0)
    this.addHarmonic(20, 15, 1.0, 0.0)
}

Harmonics.prototype.addHarmonic =
    function(...args)
    {
        var new_harmonic = new Harmonic(++this.harmonicsCount, ...args)
        this.harmonics.push(new_harmonic)
        document.getElementById("harmonic-adjusters").append(new_harmonic.element)

        // MathJax.Hub.Queue(["Typeset", MathJax.Hub, new_div.firstChild]);
    }

Harmonics.prototype.serialize =
    function ()
    {
        return this.harmonics.map(harmonic => harmonic.serialize()).join(" ")
    }

harmonics = new Harmonics()
document.getElementById("another-harmonic").addEventListener(
    'mouseup',
    function (event)
    {
        harmonics.addHarmonic(1, 0, 1.0, 0.0)
    }
)

document.getElementById("refresh-texture").addEventListener(
    'mouseup',
    function (event)
    {
        requestNewTextures(harmonics.serialize())
    }
)

requestNewTextures(harmonics.serialize())
