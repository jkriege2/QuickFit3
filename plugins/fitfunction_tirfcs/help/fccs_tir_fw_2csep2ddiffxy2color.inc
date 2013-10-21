<p>This is a simplified version of the standard 2-color TIR-FCCS fit functions. It implements the following equations:
  $$bmath:g_\text{gg}(\tau)&=\frac{\eta_\text{g}^2c_\text{A}+\eta_\text{g}^2c_\text{AB}}{\eta_\text{g}^2\cdot(c_\text{A}+c_\text{AB})^2}\cdot G_\text{gg}(\tau; D_1^\text{gg},D_2^\text{gg})$$
  $$bmath:g_\text{rr}(\tau)&=\frac{\eta_\text{r}^2\cdot\left[c_\text{B}+c_\text{AB}\right]+\kappa^2\eta_\text{g}^2\cdot\left[c_\text{A}+c_\text{AB}\right]+2\kappa\eta_\text{r}\eta_\text{g} c_\text{AB}}{\left(\kappa\eta_\text{g}c_\text{A}+(\eta_\text{r}+\kappa\eta_\text{g})\cdot c_\text{AB}+\eta_\text{r}c_\text{B}\right)^2}\cdot G_\text{rr}(\tau; D_1^\text{rr},D_2^\text{rr})$$
  $$bmath:g_\text{gr}(\tau)&=g_\text{rg}(\tau)=\frac{\eta_\text{g}\eta_\text{r} c_\text{AB}+\kappa\eta_\text{g}\eta_\text{r} c_\text{A}+\kappa\eta_\text{g}^2\cdot c_\text{AB}}{\left(\eta_\text{g}c_\text{A}+\eta_\text{g}c_\text{AB}\right)\cdot\left(\kappa\eta_\text{g}c_\text{A}+(\eta_\text{r}+\kappa\eta_\text{g})c_\text{AB}+\eta_\text{r}c_\text{B}\right)}\cdot G_\text{gr}(\tau; D_1^\text{gr},D_2^\text{gr})$$
  with
  $$bmath:G_{\gamma\rho}(\tau; D_1^{\gamma\rho},D_2^{\gamma\rho})= (1-p_{\gamma\rho,2})\cdot \frac{1}{a^2}\cdot\left\{\erf\left(\frac{\sqrt{2}\cdot a}{\sqrt{8D_1^{\gamma\rho}+w_\gamma^2+w_\rho^2}}\right)+\frac{\sqrt{8D_1^{\gamma\rho}+w_\gamma^2+w_\rho^2}}{a\cdot\sqrt{2\pi}}\cdot\left[e^{-\frac{2\cdot a^2}{8D_1^{\gamma\rho}\tau+w_\gamma^2+w_\rho^2}}-1\right]\right\}^2\cdot\frac{\sqrt{{2}/{\pi}}}{\sqrt{8D_1^{\gamma\rho}\tau+z_\gamma^2+z_\rho^2}}+p_{\gamma\rho,2}\cdot \frac{1}{a^2}\cdot\left\{\erf\left(\frac{\sqrt{2}\cdot a}{\sqrt{8D_2^{\gamma\rho}+w_\gamma^2+w_\rho^2}}\right)+\frac{\sqrt{8D_2^{\gamma\rho}+w_\gamma^2+w_\rho^2}}{a\cdot\sqrt{2\pi}}\cdot\left[e^{-\frac{2\cdot a^2}{8D_2^{\gamma\rho}\tau+w_\gamma^2+w_\rho^2}}-1\right]\right\}^2 $$
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
    <tr><td>$$math:D_{1}^{\gamma\rho}$$</i></td><td>diffusion coefficient 1 of the channel $$math:\gamma\rho$$</td></tr>
    <tr><td>$$math:D_{2}^{\gamma\rho}$$</i></td><td>diffusion coefficient 2 of the channel $$math:\gamma\rho$$</td></tr>
    <tr><td>$$math:p_{\gamma\rho,2}$$</i></td><td>fraction of diffusing component 2 in the channel $$math:\gamma\rho$$</td></tr>
    <tr><td>$$math:a$$</td><td>width of a (square) pixel in the image plane (i.e. pixel size of camera divided by the magnification)</td></tr>
    <tr><td>$$math:w_g$$</td><td>lateral 1/e²-radius of the point-spread function (PSF) of the green focus</td></tr>
    <tr><td>$$math:w_r$$</td><td>lateral 1/e²-radius of the point-spread function (PSF) of the red focus</td></tr>
    <tr><td>$$math:d_x$$</i></td><td>lateral displacement in x-direction of the foci</td></tr>
    <tr><td>$$math:d_y$$</i></td><td>lateral displacement in y-direction of the foci</td></tr>
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