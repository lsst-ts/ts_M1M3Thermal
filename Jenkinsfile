#!/usr/bin/env groovy

properties(
    [
    buildDiscarder
        (logRotator (
            artifactDaysToKeepStr: '',
            artifactNumToKeepStr: '',
            daysToKeepStr: '14',
            numToKeepStr: ''
        ) ),
    disableConcurrentBuilds(),
    parameters
        ( [
            booleanParam(defaultValue: false, description: 'Adds --no-cache to Docker build command', name: 'noCache'),
            booleanParam(defaultValue: false, description: 'Calls make clean before building the code', name: 'clean')
        ] )
    ]
)

node {
    def SALUSER_HOME = "/home/saluser"
    def BRANCH = (env.CHANGE_BRANCH != null) ? env.CHANGE_BRANCH : env.BRANCH_NAME
    def SAME_CRIO_BRANCH = ["main"]
    def CRIO_BRANCH=(BRANCH in SAME_CRIO_BRANCH) ? BRANCH : "develop"

    stage('Cloning sources')
    {
        dir("ts_m1m3thermal") {
            checkout scm
        }
    }

    stage('Building dev container (with tests)')
    {
        M1M3sim = docker.build(
            "lsstts/mtm1m3_sim:" + env.BRANCH_NAME.replace("/", "_"),
            "--target crio-develop --build-arg XML_BRANCH=main "
            + "--build-arg KAFKA_HOST=$LSST_KAFKA_HOST --build-arg KAFKA_BROKER_PORT=$LSST_KAFKA_BROKER_PORT "
            + "--build-arg SCHEMA_REGISTRY_URI=$LSST_SCHEMA_REGISTRY_URL "
            + "--build-arg cRIO_CPP=$CRIO_BRANCH --build-arg M1M3_THERMAL=$BRANCH "
            + "--build-arg TARGET=junit "
            + (params.noCache ? "--no-cache " : " ") + "$WORKSPACE/ts_m1m3thermal"
        )
    }

    stage("Copying test results")
    {
        withEnv(["SALUSER_HOME=" + SALUSER_HOME]) {
             M1M3sim.inside("--entrypoint=''") {
                 sh """
                    cp -v $SALUSER_HOME/ts_m1m3thermal/tests/*.xml $WORKSPACE/ts_m1m3thermal/tests
                 """
             }
        }

        junit 'ts_m1m3thermal/tests/*.xml'
    }

    stage('Build documentation')
    {
         M1M3sim.inside("--entrypoint=''") {
             sh """
                source $SALUSER_HOME/.crio_setup.sh

                mamba install -y doxygen
                cd $SALUSER_HOME/ts_m1m3thermal
                make doc
             """
         }
    }

    if (BRANCH == "main" || BRANCH == "develop")
    {
        stage('Publish documentation')
        {
            withCredentials([usernamePassword(credentialsId: 'lsst-io', usernameVariable: 'LTD_USERNAME', passwordVariable: 'LTD_PASSWORD')]) {
                M1M3sim.inside("--entrypoint=''") {
                    sh """
                        source $SALUSER_HOME/.crio_setup.sh
                        ltd upload --product ts-m1m3thermal --git-ref """ + BRANCH + """ --dir $SALUSER_HOME/ts_m1m3thermal/doc/html
                    """
                }
            }
        }
    }
}
