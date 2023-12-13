

1. Currently, there are only two demo supported on the RV1106/1103 platform，located under the diretory of examples/RV1106_RV1103. **The RV1106 build script is referring to RV1106/RV1103 in the demo**.

2. For all demos, except for the one under examples/RV1106_RV1103, they are all supported on the platform of RK356X/RK3588.

3. **When setting LD_LIBRARY_PATH on the RV1106 or RV1103**, it must be the full path. For example,

   ```shell
   export LD_LIBRARY_PATH=/userdata/lib
   ```

   For RK356X, RK3588, LD_LIBRARY_PATH can be set as either full path or relative path


