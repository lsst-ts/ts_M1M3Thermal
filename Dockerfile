FROM lsstts/develop-env:develop as crio-develop

ARG XML_BRANCH=develop

USER root
RUN chmod a+rwX -R /home/saluser/
USER saluser

ARG XML_BRANCH=main

WORKDIR /home/saluser

RUN source ~/.setup.sh \
    && mamba install -y readline yaml-cpp catch2 spdlog \
    && echo > .crio_setup.sh -e \
echo "Configuring cRIO development environment" \\n\
export SHELL=bash \\n\
source /home/saluser/.setup_salobj.sh \\n\
export PATH=\$CONDA_PREFIX/bin:\$PATH \\n\
export LIBS="-L\$CONDA_PREFIX/lib" \\n\
export CPP_FLAGS="-I\$CONDA_PREFIX/include" \\n

RUN source ~/.crio_setup.sh && cd $TS_XML_DIR \
    && ~/.checkout_repo.sh $XML_BRANCH develop && git pull \
    && salgeneratorKafka generate cpp MTM1M3TS

FROM crio-develop

ARG cRIO_CPP=v1.11.0
ARG M1M3_THERMAL=develop
ARG TARGET="SIMULATOR=1"

RUN cd repos && git clone --branch $cRIO_CPP https://github.com/lsst-ts/ts_cRIOcpp
RUN source ~/.crio_setup.sh && cd repos/ts_cRIOcpp && make

RUN cd repos && git clone --branch $M1M3_THERMAL https://github.com/lsst-ts/ts_m1m3thermal
RUN source ~/.crio_setup.sh && cd repos/ts_m1m3thermal && make $TARGET

SHELL ["/bin/bash", "-lc"]
