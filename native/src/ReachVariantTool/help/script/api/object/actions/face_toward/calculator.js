
class FaceTowardCalculator extends HTMLElement {
   static #TAG_NAME = "face-toward-calculator";

   #initialized = false;

   constructor() {
      super();
   }
   connectedCallback() {
      if (this.#initialized)
         return;
      this.#initialized = true;

      this.innerHTML = `
<label>Degrees: <input name="deg" type="number" min="-360" max="360" step="1" value="0" size="7" /></label>
<span>&#x1F846;</span>
<label>X: <input readonly name="x" type="number" min="-128" max="127" step="1" size="7" /></label>
<label>Y: <input readonly name="y" type="number" min="-128" max="127" step="1" size="7" /></label>
      `.trim();

      this.querySelector("[name=deg]").addEventListener("change", (function (e) {
         let x_node = this.querySelector("[name=x]");
         let y_node = this.querySelector("[name=y]");

         let value = Math.floor(+e.target.value || 0);
         while (value < 0)
            value += 360;
         value = value % 360;

         let info = this.constructor.#lookup_table[value];
         x_node.value = info.x;
         y_node.value = info.y;
      }).bind(this));
   }
   static #lookup_table = (function() {
      const RADIANS_TO_DEGREES = 57.295779513082320876798154814105170332405472466564;

      let result = Array(359).fill(0);

      for (let y = -128; y <= 127; ++y) {
         for (let x = -128; x <= 127; ++x) {
            let degrees = Math.atan2(y, x) * RADIANS_TO_DEGREES;
            let deg_pos = Math.abs(degrees);
            let target  = Math.floor(deg_pos);

            let error = deg_pos - target;
            if (error > 0.5) {
               error   = 1 - error;
               target += 1;
            }

            if (!result[target] || result[target].error > error) {
               result[target] = {
                  x:     x,
                  y:     y,
                  error: error
               };
            }
         }
      }

      return result;
   })();

   static {
      window.customElements.define(this.#TAG_NAME, this);
   }
};