<h2>Basic SPIM-FCS correlation functions</h2>
<h3>FCCS framework</h3>
$$insert:framework_schwille.inc$$
<h3>The confocal MDF and correlation function</h3>
We assume the MDF of a confocal microscope to begaussian:$$REF_WOHLANDSPIMFCS$$ $$REF_SPIMFCCS$$ $$jankrieger_phdthesis_ref$$
  $$bmath:\mbox{MDF}_\text{g/r}(x,y,z)= \frac{1}{\mathcal{N}}\cdot \exp\left(-2\cdot\frac{x^2+y^2}{w_{g/r}^2}-2\cdot\frac{z^2}{z_{g/r}^2}\right)$$
Then the non-normalized crosscorrelation function for species X and between channels g and r can be written as:
  $$bmath:G_\text{gr}^{X}(\tau)=\eta_\text{g}\eta_\text{r}c_\text{X}\cdot G_\text{gr,x}^{X}(\tau)\cdot G_\text{gr,y}^{X}(\tau)\cdot G_\text{gr,z}^{X}(\tau)$$
where for a confocal microscope the x- and y-components are equal:
  $$bmath:G_\text{gr,y}^{X}(\tau)\ =G_\text{gr,x}^{X}(\tau)\ =\ \int_{-\infty}^\infty\int_{-\infty}^\infty\mbox{MDF}_\text{g}(x)\cdot\mbox{MDF}_\text{r}(x')\cdot \frac{1}{\sqrt{4\pi D_X\tau}}\cdot\exp\left[-\frac{(x-x')^2}{4D_X\tau}\right]\;\mathrm{d}x\;\mathrm{d}x'\ =\ \sqrt{\frac{2}{\pi}}\cdot\frac{\exp\left[-\frac{2\cdot d_x^2}{8D_X\tau+w_g^2+w_r^2}\right]}{\sqrt{8D_X\tau+w_g^2+w_r^2}}.$$
and the z-component is:
  $$bmath:G_\text{gr,z}^{X}(\tau)\ =\ \int_{-\infty}^\infty\int_{-\infty}^\infty\mbox{MDF}_\text{g}(z)\cdot\mbox{MDF}_\text{r}(z')\cdot \frac{1}{\sqrt{4\pi D_X\tau}}\cdot\exp\left[-\frac{(z-z')^2}{4D_X\tau}\right]\;\mathrm{d}z\;\mathrm{d}z'\ =\ \sqrt{\frac{2}{\pi}}\cdot\frac{\exp\left[-\frac{2\cdot d_z^2}{8D_X\tau+z_g^2+z_r^2}\right]}{\sqrt{8D_X\tau+z_g^2+z_r^2}}.$$

<h3>Adding a second component</h3>
In contrast to the 1-component diffusion model as described above, here the single component CFs $$math:G^\chi(\tau)\equiv G^\chi(\tau; D_\chi)$$ which only depend on one diffusion coefficient $$math:D_\chi$$ per species $$math:\chi$$ are replaced by a two-component CF:
    $$bmath:G^\chi(\tau)\rightarrow \rho_{\chi,1}\cdot G^\chi(\tau; D_{\chi,1})+(1-\rho_{\chi,1})\cdot G^\chi(\tau; D_{\chi,2})$$
  Here $$math:\rho_{\chi,1}\in[0,1]$$ is the fraction of the first diffusion component and $$math:\rho_{\chi,2}=(1-\rho_{\chi,1})$$. This means that we still assume that only three different species are present in the sample, but that these each show two distinct diffusion components.
