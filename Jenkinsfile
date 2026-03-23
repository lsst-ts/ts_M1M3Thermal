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
    def SAME_CRIO_BRANCH = ["main", "tickets/DM-46500"]
    def CRIO_BRANCH=(BRANCH in SAME_CRIO_BRANCH) ? BRANCH : "develop"

    stage('Cloning sources')
    {
        dir("ts_cRIOcpp") {
            git branch: CRIO_BRANCH, url: 'https://github.com/lsst-ts/ts_cRIOcpp'
        }
        dir("ts_m1m3thermal") {
            checkout scm
        }
    }

    stage('Building dev container')
    {
        M1M3sim = docker.build(
            "lsstts/mtm1m3_sim:" + env.BRANCH_NAME.replace("/", "_"),
            "--target crio-develop --build-arg XML_BRANCH=main "
            + "--build-arg KAFKA_HOST=$LSST_KAFKA_HOST --build-arg KAFKA_BROKER_PORT=$LSST_KAFKA_BROKER_PORT "
            + "--build-arg SCHEMA_REGISTRY_URI=$LSST_SCHEMA_REGISTRY_URL "
            + "--build-arg cRIO_CPP=$CRIO_BRANCH --build-arg M1M3_THERMAL=$BRANCH"
            + (params.noCache ? " --no-cache " : " ") + "$WORKSPACE/ts_m1m3thermal"
        )
    }

    stage("Running tests")
    {
        withEnv(["SALUSER_HOME=" + SALUSER_HOME]) {
             M1M3sim.inside("--entrypoint=''") {
                 if (params.clean) {
                 sh """
                    cd $WORKSPACE/ts_cRIOcpp
                    make clean
                    cd $WORKSPACE/ts_m1m3thermal
                    make clean
                 """
                 }
                 sh """
                    source $SALUSER_HOME/.crio_setup.sh
    
                    export PATH=\$CONDA_PREFIX/bin:$PATH
                    cd $WORKSPACE/ts_cRIOcpp
                    make
    
                    cd $WORKSPACE/ts_m1m3thermal
                    make SIMULATOR=1

                    make SIMULATOR=1 junit
                 """
             }
        }

    }

    stage('Running container')
    {
        withEnv(["SALUSER_HOME=" + SALUSER_HOME]){
            M1M3sim.inside("--entrypoint=''") {
                sh """
                    source $SALUSER_HOME/.crio_setup.sh

                    create_topics MTM1M3TS

                    cd $WORKSPACE/ts_m1m3thermal
                    ./ts-M1M3thermald -c SettingFiles &
 
                    echo "Waiting for 30 seconds"
                    sleep 30

                    pytest --junit-xml=tests/test_CSC.xml tests

                    sleep 30
                    killall ts-M1M3thermald
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
                cd $WORKSPACE/ts_m1m3thermal
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
                        ltd upload --product ts-m1m3thermal --git-ref """ + BRANCH + """ --dir $WORKSPACE/ts_m1m3thermal/doc/html
                    """
                }
            }
        }
    }
}
