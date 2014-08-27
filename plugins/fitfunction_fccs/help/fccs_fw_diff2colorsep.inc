<p>This is a simplified version of the standard 2-color SPIM-FCCS fit functions. $$REF_WOHLANDSPIMFCS$$ $$REF_SPIMFCCS$$ $$jankrieger_phdthesis_ref$$ It implements the following equations:
  $$bmath:g_\text{gg}(\tau)=\frac{\eta_\text{g}^2c_\text{A}+\eta_\text{g}^2c_\text{AB}}{\eta_\text{g}^2\cdot(c_\text{A}+c_\text{AB})^2}\cdot G_\text{gg}(\tau; D_1^\text{gg})$$
  $$bmath:g_\text{rr}(\tau)=\frac{\eta_\text{r}^2\cdot\left[c_\text{B}+c_\text{AB}\right]+\kappa^2\eta_\text{g}^2\cdot\left[c_\text{A}+c_\text{AB}\right]+2\kappa\eta_\text{r}\eta_\text{g} c_\text{AB}}{\left(\kappa\eta_\text{g}c_\text{A}+(\eta_\text{r}+\kappa\eta_\text{g})\cdot c_\text{AB}+\eta_\text{r}c_\text{B}\right)^2}\cdot G_\text{rr}(\tau; D_1^\text{rr})$$
  $$bmath:g_\text{gr}(\tau)=g_\text{rg}(\tau)=\frac{\eta_\text{g}\eta_\text{r} c_\text{AB}+\kappa\eta_\text{g}\eta_\text{r} c_\text{A}+\kappa\eta_\text{g}^2\cdot c_\text{AB}}{\left(\eta_\text{g}c_\text{A}+\eta_\text{g}c_\text{AB}\right)\cdot\left(\kappa\eta_\text{g}c_\text{A}+(\eta_\text{r}+\kappa\eta_\text{g})c_\text{AB}+\eta_\text{r}c_\text{B}\right)}\cdot G_\text{gr}(\tau; D_1^\text{gr})$$
  with
  $$bmath:G_\text{gr}^{X}(\tau)=G_\text{gr,x}^{X}(\tau)\cdot G_\text{gr,y}^{X}(\tau)\cdot G_\text{gr,z}^{X}(\tau)$$
where for a confocal microscope the x- and y-components are equal:
  $$bmath:G_\text{gr,y}^{X}(\tau)\ =G_\text{gr,x}^{X}(\tau)\ =\ \int_{-\infty}^\infty\int_{-\infty}^\infty\mbox{MDF}_\text{g}(x)\cdot\mbox{MDF}_\text{r}(x')\cdot \frac{1}{\sqrt{4\pi D_X\tau}}\cdot\exp\left[-\frac{(x-x')^2}{4D_X\tau}\right]\;\mathrm{d}x\;\mathrm{d}x'\ =\ \sqrt{\frac{2}{\pi}}\cdot\frac{\exp\left[-\frac{2\cdot d_x^2}{8D_X\tau+w_g^2+w_r^2}\right]}{\sqrt{8D_X\tau+w_g^2+w_r^2}}.$$
and the z-component is:
  $$bmath:G_\text{gr,z}^{X}(\tau)\ =\ \int_{-\infty}^\infty\int_{-\infty}^\infty\mbox{MDF}_\text{g}(z)\cdot\mbox{MDF}_\text{r}(z')\cdot \frac{1}{\sqrt{4\pi D_X\tau}}\cdot\exp\left[-\frac{(z-z')^2}{4D_X\tau}\right]\;\mathrm{d}z\;\mathrm{d}z'\ =\ \sqrt{\frac{2}{\pi}}\cdot\frac{\exp\left[-\frac{2\cdot d_z^2}{8D_X\tau+z_g^2+z_r^2}\right]}{\sqrt{8D_X\tau+z_g^2+z_r^2}}.$$

  
  
  $$bmath:\eta_g=\frac{I_g-B_g}{c_\text{A}+c_\text{AB}$$
  $$bmath:\eta_r=\frac{I_r-B_r}{c_\text{B}+c_\text{AB}$$
  </p>
  
  <p>  The parameters are:
  <blockquote><table border=1>
    <tr><th>var.</th><th>Parameter</th></tr>
    <tr><td><i>G<sub>&infin;</sub></i></td><td>constant offset</td></tr>
    <tr><td>$$math:\kappa$$</td><td>crosstalk of the green dye (species A and AB) into the red channel</td></tr>
    <tr><td>$$math:c_{A}$$</td><td>particle concentration of species a the observation volume</td></tr>
    <tr><td>$$math:c_{B}$$</td><td>particle concentration of species b the observation volume</td></tr>
    <tr><td>$$math:c_{AB}$$</td><td>particle concentration of the crosscorrelating species the observation volume</td></tr>
    <tr><td>$$math:D_{X}^{\gamma\rho}$$</i></td><td>diffusion coefficient of the channel $$math:\gamma\rho$$</td></tr>
    <tr><td>$$math:w_g$$</td><td>lateral 1/e²-radius of the point-spread function (PSF) of the green focus</td></tr>
    <tr><td>$$math:w_r$$</td><td>lateral 1/e²-radius of the point-spread function (PSF) of the red focus</td></tr>
    <tr><td>$$math:\gamma_g=z_g/w_g$$</td><td>axial ratio of the point-spread function (PSF) of the green focus</td></tr>
    <tr><td>$$math:\gamma_r=z_r/w_r$$</td><td>axial ratio of the point-spread function (PSF) of the red focus</td></tr>
    <tr><td>$$math:d_x$$</i></td><td>lateral displacement in x-direction of the foci</td></tr>
    <tr><td>$$math:d_y$$</i></td><td>lateral displacement in y-direction of the foci</td></tr>
    <tr><td>$$math:d_z$$</i></td><td>longitudinal displacement in z-direction of the foci</td></tr>
    <tr><td>$$math:I_g$$</td><td>average measured intensity in focus 1 during measurement</td></tr>
    <tr><td>$$math:B_g$$</td><td>average background intenisty in focus 1 during measurement</td></tr>
    <tr><td>$$math:I_g$$</td><td>average measured intensity in focus 2 during measurement</td></tr>
    <tr><td>$$math:B_g$$</td><td>average background intenisty in focus 2 during measurement</td></tr>
</table>  </blockquote>
</p>

  
  <p>
    The idea behind this model is that each correlation function (ACF0, ACF1, CCF) is fitted with it's own two diffusing components, without assigning each of the species it's own diffusion coefficient. This makes the models way simpler and removes parameters, so the model converges easier in a global fit where the concentrations are linked of the three correlation functions.
    $$note:Due to this simplification, it is (in most cases) no longer possible to assign the diffusion coefficients of a channel directly to any of the species present in the sample.$$
  </p>