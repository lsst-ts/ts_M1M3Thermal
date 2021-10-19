FROM lsstts/develop-env:develop

USER root
RUN yum -y install catch-devel boost169-devel make readline-devel

USER saluser

ARG XML_BRANCH

WORKDIR /home/saluser/repos/ts_xml
RUN git pull && git checkout $XML_BRANCH && git pull

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
    && conda install -y readline yaml-cpp boost-cpp

SHELL ["/bin/bash", "-lc"]
