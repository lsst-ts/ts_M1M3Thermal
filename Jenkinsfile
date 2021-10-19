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
    def SAME_CRIO_BRANCH = ["master", "tickets/DM-31838", "tickets/DM-32240"]

    stage('Cloning Dockerfile')
    {
        sh "printenv"

        dir("ts_Dockerfiles") {
            git branch: (BRANCH == "master" ? "master" : "develop"), url: 'https://github.com/lsst-ts/ts_Dockerfiles'
        }
    }

    stage('Building dev container')
    {
        M1M3sim = docker.build("lsstts/mtm1m3_sim:" + env.BRANCH_NAME.replace("/", "_"), (params.noCache ? "--no-cache " : " ") + "--target lsstts-cpp-dev --build-arg XML_BRANCH=HEAD ts_Dockerfiles/mtm1m3_sim")
    }

    stage('Cloning sources')
    {
        dir("ts_cRIOcpp") {
            git branch: (BRANCH in SAME_CRIO_BRANCH) ? BRANCH : "develop", url: 'https://github.com/lsst-ts/ts_cRIOcpp'
        }
        dir("ts_m1m3thermal") {
            checkout scm
        }
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
                    source $SALUSER_HOME/.setup_salobj.sh
    
                    export PATH=\$CONDA_PREFIX/bin:$PATH
                    cd $WORKSPACE/ts_cRIOcpp
                    make
    
                    cd $WORKSPACE/ts_m1m3thermal
                    LIBS_FLAGS="-L\$CONDA_PREFIX/lib" make simulator
                    make junit
                 """
             }
        }

        junit 'ts_m1m3thermal/tests/*.xml'
    }

    stage('Build documentation')
    {
         M1M3sim.inside("--entrypoint=''") {
             sh """
                source $SALUSER_HOME/.setup_salobj.sh
                cd $WORKSPACE/ts_m1m3thermal
                make doc
             """
         }
    }

    stage('Running container')
    {
        withEnv(["SALUSER_HOME=" + SALUSER_HOME]){
            M1M3sim.inside("--entrypoint=''") {
                sh """
                    source $SALUSER_HOME/.setup_salobj.sh

                    export LSST_DDS_PARTITION_PREFIX=test
    
                    cd $WORKSPACE/ts_m1m3thermal
                    ./ts-M1M3thermald -c SettingFiles &
    
                    echo "Waiting for 30 seconds"
                    sleep 30
    
                    cd $SALUSER_HOME/repos
                    ./ts_sal/test/MTM1M3TS/cpp/src/sacpp_MTM1M3TS_start_commander Default
                    sleep 30
                    killall ts-M1M3thermald
                """
            }
        }
    }

    if (BRANCH == "master" || BRANCH == "develop")
    {
        stage('Publish documentation')
        {
            withCredentials([usernamePassword(credentialsId: 'lsst-io', usernameVariable: 'LTD_USERNAME', passwordVariable: 'LTD_PASSWORD')]) {
                M1M3sim.inside("--entrypoint=''") {
                    sh """
                        source $SALUSER_HOME/.setup_salobj.sh
                        ltd upload --product ts-m1m3thermal --git-ref """ + BRANCH + """ --dir $WORKSPACE/ts_m1m3thermal/doc/html
                    """
                }
            }
        }
    }
}
