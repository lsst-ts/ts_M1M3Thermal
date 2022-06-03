FROM lsstts/develop-env:c0025.005

USER root
RUN chmod a+rwX -R /home/saluser/
USER saluser

ARG XML_BRANCH

WORKDIR /home/saluser/repos/ts_xml
RUN git fetch && git checkout $XML_BRANCH && git pull

WORKDIR /home/saluser
RUN source .setup.sh \
    && cd repos/ts_sal \
    && cp ../ts_xml/sal_interfaces/*.xml ../ts_xml/sal_interfaces/MTM1M3TS/*.xml test \
    && cd test \
    && salgenerator generate cpp MTM1M3TS

USER root
RUN chmod a+rwX -R /home/saluser/
USER saluser

RUN source .setup.sh \
    && mamba install -y readline yaml-cpp boost-cpp catch2

SHELL ["/bin/bash", "-lc"]
