node ('Qt5.6.0&&VS2015') {
   
   
   // Mark the code checkout 'stage'....
   stage 'Checkout' {

     // Cleanup
     gitClean()

     // Checkout code from repository
     checkout scm

     // Update and fetch submodules
     bat 'git submodule foreach --recursive git reset --hard'
     bat 'git submodule foreach --recursive git clean -fdx'

   }

   // Mark the code build 'stage'....
   stage 'Build'
   bat "echo \"Hello\" "
}
